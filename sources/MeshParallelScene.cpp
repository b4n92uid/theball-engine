/*
 * File:   MeshParallelScene.cpp
 * Author: b4n92uid
 *
 * Created on 30 avril 2010, 23:02
 */

#include "MeshParallelScene.h"
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
}

MeshParallelScene::~MeshParallelScene()
{
}

struct DepthSortMeshFunc
{

    bool operator()(Mesh* node1, Mesh * node2)
    {
        if(node1->isTransparent() && node2->isTransparent())
            return(node1->getAbsoluteMatrix().getPos() - camPos) > (node2->getAbsoluteMatrix().getPos() - camPos);

        else if(node1->isTransparent() && !node2->isTransparent())
            return false;

        else if(!node1->isTransparent() && node2->isTransparent())
            return true;

        else
            return(node1->getAbsoluteMatrix().getPos() - camPos) < (node2->getAbsoluteMatrix().getPos() - camPos);
    }

    Vector3f camPos;
};

void MeshParallelScene::render()
{
    Frustum* frustum = m_sceneManager->getFrustum();

    m_frustumCullingCount = 0;

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
    }
}

Mesh::Array MeshParallelScene::findMeshs(Vector3f start, Vector3f diri)
{
    Mesh::Array matches;

    for(unsigned i = 0; i < m_nodes.size(); i++)
    {
        float intersect;

        if(m_nodes[i]->isEnable() && m_nodes[i]->isAttached())
            if(m_nodes[i]->rayCast(start, diri, intersect, true))
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

void MeshParallelScene::setFrustumCullingCount(unsigned frustumCullingCount)
{
    this->m_frustumCullingCount = frustumCullingCount;
}

unsigned MeshParallelScene::getFrustumCullingCount() const
{
    return m_frustumCullingCount;
}
