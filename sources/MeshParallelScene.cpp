/*
 * File:   MeshParallelScene.cpp
 * Author: b4n92uid
 *
 * Created on 30 avril 2010, 23:02
 */

#include <queue>

#include "MeshParallelScene.h"
#include "Light.h"
#include "SceneManager.h"
#include "Frustum.h"
#include "Mesh.h"
#include "Tools.h"
#include "ObjMesh.h"
#include "ShadowMap.h"
#include "VolumetricLight.h"

using namespace tbe;
using namespace tbe::scene;

MeshParallelScene::MeshParallelScene()
{
    m_frustumCullingCount = 0;
    m_enableFrustumTest = true;
    m_transparencySort = false;

    glGetIntegerv(GL_MAX_LIGHTS, & m_maxlight);
}

MeshParallelScene::~MeshParallelScene() { }

struct RenderQueue
{
    SubMesh* render;
    std::vector<Matrix4> transforms;
};

struct DepthSortMeshFunc
{

    bool operator()(const RenderQueue& rq1, const RenderQueue& rq2)
    {
        Material* mat1 = rq1.render->getMaterial();
        Material* mat2 = rq2.render->getMaterial();
        Mesh* node1 = rq1.render->getOwner();
        Mesh* node2 = rq2.render->getOwner();

        if(node1->getPriorityRender() != 0 || node2->getPriorityRender() != 0)
            return node1->getPriorityRender() > node2->getPriorityRender();

        else if(mat1->isTransparent() && mat2->isTransparent())
            return (node1->getAbsoluteMatrix().getPos() - camPos) > (node2->getAbsoluteMatrix().getPos() - camPos);

        else if(mat1->isTransparent() && !mat2->isTransparent())
            return false;

        else if(!mat1->isTransparent() && mat2->isTransparent())
            return true;

        else
            return (node1->getAbsoluteMatrix().getPos() - camPos) < (node2->getAbsoluteMatrix().getPos() - camPos);
    }

    Vector3f camPos;
};

void MeshParallelScene::drawScene(bool shadowpass)
{
    using namespace std;

    Frustum* frustum = m_sceneManager->getFrustum();

    m_frustumCullingCount = 0;
    m_renderedMeshCount = 0;

    std::vector<RenderQueue> renderqueue;

    // Pre allocate approximate nodes count
    renderqueue.reserve(m_nodes.size());

    BOOST_FOREACH(Mesh* node, m_nodes)
    {
        if(!node->isAttached() || !node->isVisible() || !node->isEnable())
            continue;

        if(m_enableFrustumTest && !frustum->isInside(node))
        {
            m_frustumCullingCount++;
            continue;
        }

        if(shadowpass && !node->isCastShadow())
            continue;

        BOOST_FOREACH(SubMesh* sm, node->getAllSubMesh())
        {
            bool pushRequest = true;
            BOOST_FOREACH(RenderQueue& rq, renderqueue)
            if(*rq.render == *sm)
            {
                rq.transforms.push_back(node->getAbsoluteMatrix());
                pushRequest = false;
                break;
            }

            if(pushRequest)
            {
                RenderQueue rq;
                rq.render = sm;
                rq.transforms.reserve(8);
                rq.transforms.push_back(node->getMatrix());
                renderqueue.push_back(rq);
            }
        }

        m_renderedMeshCount++;
    }

    if(m_transparencySort)
    {
        DepthSortMeshFunc sortFunc;
        sortFunc.camPos = m_sceneManager->getCurCamera()->getPos();
        std::sort(renderqueue.begin(), renderqueue.end(), sortFunc);
    }

    if(shadowpass) BOOST_FOREACH(RenderQueue & rq, renderqueue)
    {
        // cout << "Render " << rq.render->getMaterial()->getName() << endl;

        rq.render->beginShadowPass();

        BOOST_FOREACH(Matrix4 & mat, rq.transforms) rq.render->drawShadow(mat);

        rq.render->endShadowPass();
    }
    else BOOST_FOREACH(RenderQueue & rq, renderqueue)
    {
        // cout << "Render " << rq.render->getMaterial()->getName() << endl;

        rq.render->bindBuffers();
        rq.render->beginProperty();

        BOOST_FOREACH(Matrix4 & mat, rq.transforms) rq.render->draw(mat);

        rq.render->endProperty();
        rq.render->unbindBuffers();
    }
}

void MeshParallelScene::render()
{
    using namespace std;

    if(!m_enable)
        return;

    vector<ShadowMap*> shadowmaps;
    vector<VolumetricLight*> volumelights;

    if(ShadowMap::enable)
    {
        // Process ShadowMap

        BOOST_FOREACH(Light* l, m_lightNodes)
        {
            if(l->getType() != Light::DIRI || !l->isCastShadow() || !l->isEnable())
                continue;

            ShadowMap* shadowMap = l->getShadowMap();

            // First pass
            shadowMap->begin();
            drawScene(true);
            shadowMap->end();

            shadowmaps.push_back(shadowMap);
        }
    }

    if(VolumetricLight::enable)
    {
        // Process VolumetricLight

        BOOST_FOREACH(Light* l, m_lightNodes)
        {
            if(!l->isEnable() || !l->isCastRays())
                continue;

            VolumetricLight* vl = l->getVolumeLight();

            vl->begin();

            vl->drawLight();
            drawScene(true);

            vl->end();

            volumelights.push_back(vl);
        }
    }

    // Render Scene
    drawScene();

    if(ShadowMap::enable)
    {
        // Post-Process ShadowMap

        BOOST_FOREACH(ShadowMap* shadowMap, shadowmaps)
        {
            if(shadowMap->isShaderHandled())
                continue;

            shadowMap->bind();
            drawScene(true);
            shadowMap->unbind();

            shadowMap->render();
        }
    }

    if(VolumetricLight::enable)
    {
        // Post-Process VolumetricLight

        BOOST_FOREACH(VolumetricLight* vl, volumelights)
        {
            vl->render();
        }
    }
}

Vector3f::Array MeshParallelScene::rayCast(Vector3f start, Vector3f dir)
{
    std::vector<float> fhits;

    // TODO Improve by frustum culling 

    for(unsigned i = 0; i < m_nodes.size(); i++)
    {
        float intersect;

        if(m_nodes[i]->isEnable() && m_nodes[i]->isAttached())
            if(m_nodes[i]->rayCast(start, dir, intersect, true))
                fhits.push_back(intersect);
    }

    std::sort(fhits.begin(), fhits.end());

    Vector3f::Array hits;
    hits.resize(fhits.size());

    for(unsigned i = 0; i < fhits.size(); i++)
        hits[i] = start + dir * fhits[i];

    return hits;
}

Mesh::Array MeshParallelScene::findMeshs(Vector3f start, Vector3f dir)
{
    Mesh::Array matches;

    for(unsigned i = 0; i < m_nodes.size(); i++)
    {
        float intersect;

        if(m_nodes[i]->isEnable() && m_nodes[i]->isAttached())
            if(m_nodes[i]->rayCast(start, dir, intersect, true))
                matches.push_back(m_nodes[i]);
    }

    return matches;
}

bool MeshParallelScene::findFloor(float x, float& y, float z)
{
    bool atleastone = false;

    float yfloor = getSceneAabb().min.y - 1;

    for(unsigned i = 0; i < m_nodes.size(); i++)
    {
        if(!m_nodes[i]->isEnable() || !m_nodes[i]->isAttached())
            continue;

        float lastY = yfloor;

        if(m_nodes[i]->findFloor(x, yfloor, z, true))
        {
            if(yfloor < lastY)
            {
                atleastone = true;
                yfloor = lastY;
            }
        }
    }

    y = yfloor;
    return atleastone;
}

void MeshParallelScene::setInFloor(Node* node)
{
    Vector3f pos = node->getPos();

    bool atleastone = false;

    pos.y = getSceneAabb().min.y - 1;

    for(unsigned i = 0; i < m_nodes.size(); i++)
    {
        if(m_nodes[i] == node || !m_nodes[i]->isEnable() || !m_nodes[i]->isAttached())
            continue;

        float yfloor;

        if(m_nodes[i]->findFloor(pos.x, yfloor, pos.z, true))
        {
            if(pos.y < yfloor)
            {
                atleastone = true;
                pos.y = yfloor;
            }
        }
    }

    if(atleastone)
        node->setPos(pos);
}

AABB MeshParallelScene::getSceneAabb()
{
    AABB sceneAabb;

    for(unsigned i = 0; i < m_nodes.size(); i++)
        if(m_nodes[i]->isAttached())
            sceneAabb.count(m_nodes[i]);

    return sceneAabb;
}

void MeshParallelScene::setTransparencySort(bool transparencySort)
{
    this->m_transparencySort = transparencySort;
}

bool MeshParallelScene::isTransparencySort() const
{
    return m_transparencySort;
}

void MeshParallelScene::setEnableFrustumTest(bool enableFrustumTest)
{
    this->m_enableFrustumTest = enableFrustumTest;
}

bool MeshParallelScene::isEnableFrustumTest() const
{
    return m_enableFrustumTest;
}

unsigned MeshParallelScene::getFrustumCullingCount() const
{
    return m_frustumCullingCount;
}

unsigned MeshParallelScene::getRenderedMeshCount() const
{
    return m_renderedMeshCount;
}

Light* MeshParallelScene::light(int index)
{
    if(index >= m_lightNodes.size())
        return NULL;

    return m_lightNodes[index];
}

void MeshParallelScene::registerLight(Light* light)
{
    if(std::find(m_lightNodes.begin(), m_lightNodes.end(), light) != m_lightNodes.end())
        throw Exception("MeshParallelScene::registerLight; child already exist");

    m_lightNodes.push_back(light);
}

void MeshParallelScene::unregisterLight(Light* light)
{
    std::vector<Light*>::iterator it = std::find(m_lightNodes.begin(), m_lightNodes.end(), light);

    if(it == m_lightNodes.end())
        throw Exception("MeshParallelScene::unregisterLight; cannot found child");

    m_lightNodes.erase(it);
}

int MeshParallelScene::beginPrePassLighting(Mesh* mesh)
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    m_prePassLights.clear();

    for(unsigned i = 0; i < m_lightNodes.size(); i++)
        if(m_lightNodes[i]->isAttached() && m_lightNodes[i]->isEnable())
        {
            if(m_lightNodes[i]->getType() == Light::DIRI)
            {
                m_prePassLights.push_back(m_lightNodes[i]);
            }

            else if(m_lightNodes[i]->getType() == Light::POINT)
            {
                Vector3f dist = m_lightNodes[i]->getAbsoluteMatrix().getPos() - mesh->getAbsoluteMatrix().getPos();
                float length = dist.getMagnitude() - m_lightNodes[i]->getRadius() - 32 - mesh->getAabb().getLength() / 2.0f;

                // radius+32 = Maximum radius of a light, beyond use a directional light

                if(length <= 0)
                    m_prePassLights.push_back(m_lightNodes[i]);
            }
        }

    return m_prePassLights.size();
}

void MeshParallelScene::prePassLighting(int i)
{
    m_prePassLights[i]->render();
}

void MeshParallelScene::endPrePassLighting()
{
    glDisable(GL_LIGHT0);
}

void MeshParallelScene::setRenderingShader(Shader renderingShader)
{
    this->m_renderingShader = renderingShader;
}

Shader MeshParallelScene::getRenderingShader() const
{
    return m_renderingShader;
}
