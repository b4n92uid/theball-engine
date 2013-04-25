/*
 * File:   MeshParallelScene.cpp
 * Author: b4n92uid
 *
 * Created on 30 avril 2010, 23:02
 */

#include "MeshParallelScene.h"
#include "Light.h"
#include "SceneManager.h"
#include "Frustum.h"
#include "Mesh.h"
#include "Tools.h"
#include "Ball3DMesh.h"
#include "ObjMesh.h"

#include <list>

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

struct DepthSortMeshFunc
{

    bool operator()(Mesh* node1, Mesh * node2)
    {
        if(node1->getPriorityRender() != 0 || node2->getPriorityRender() != 0)
            return node1->getPriorityRender() > node2->getPriorityRender();

        else if(node1->isTransparent() && node2->isTransparent())
            return (node1->getAbsoluteMatrix().getPos() - camPos) > (node2->getAbsoluteMatrix().getPos() - camPos);

        else if(node1->isTransparent() && !node2->isTransparent())
            return false;

        else if(!node1->isTransparent() && node2->isTransparent())
            return true;

        else
            return (node1->getAbsoluteMatrix().getPos() - camPos) < (node2->getAbsoluteMatrix().getPos() - camPos);
    }

    Vector3f camPos;
};

void MeshParallelScene::render()
{
    if(!m_enable)
        return;

    Frustum* frustum = m_sceneManager->getFrustum();

    m_frustumCullingCount = 0;
    m_renderedMeshCount = 0;

    static DepthSortMeshFunc sortFunc;
    sortFunc.camPos = m_sceneManager->getCurCamera()->getPos();

    if(m_transparencySort)
        std::sort(m_nodes.begin(), m_nodes.end(), sortFunc);

    for(Mesh::Array::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it)
    {
        Mesh* node = *it;

        if(!node->isAttached())
            continue;

        if(m_enableFrustumTest && !frustum->isInside(node))
        {
            m_frustumCullingCount++;
            continue;
        }

        node->render();
        m_renderedMeshCount++;

    }
}

Vector3f::Array MeshParallelScene::rayCast(Vector3f start, Vector3f dir)
{
    std::vector<float> fhits;

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
