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
}

MeshParallelScene::~MeshParallelScene()
{
}

struct DepthSortMeshFunc
{

    bool operator()(Mesh* node1, Mesh * node2)
    {
        return (node1->getPos() - camPos) > (node2->getPos() - camPos);
    }

    Vector3f camPos;
};

void MeshParallelScene::render()
{
    Frustum* frustum = m_sceneManager->getFrustum();

    m_frustumCullingCount = 0;

    static DepthSortMeshFunc sortFunc;
    sortFunc.camPos = m_sceneManager->getCurCamera()->getPos();
    std::sort(m_nodes.begin(), m_nodes.end(), sortFunc);

    for(Mesh::Array::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it)
    {
        Node* node = *it;

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

bool MeshParallelScene::findFloor(Vector3f& pos)
{
    bool atleastone = false;

    pos.y = getSceneAabb().min.y - 1;

    for(unsigned i = 0; i < m_nodes.size(); i++)
    {
        float lastY = pos.y;

        if(m_nodes[i]->findFloor(pos, true))
        {
            atleastone = true;
            pos.y = std::max(pos.y, lastY);
        }
    }

    return atleastone;
}

void MeshParallelScene::setInFloor(Node* node)
{
    Vector3f pos = node->getPos();

    pos.y = getSceneAabb().min.y - 1;

    for(unsigned i = 0; i < m_nodes.size(); i++)
    {
        if(m_nodes[i] == node || !m_nodes[i]->isEnable())
            continue;

        float lastY = pos.y;

        if(m_nodes[i]->findFloor(pos, true))
            pos.y = std::max(pos.y, lastY);
    }

    node->setPos(pos);
}

AABB MeshParallelScene::getSceneAabb()
{
    AABB sceneAabb;

    for(unsigned i = 0; i < m_nodes.size(); i++)
        sceneAabb.count(m_nodes[i]);

    return sceneAabb;
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
