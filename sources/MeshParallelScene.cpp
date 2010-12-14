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
    Clear();
}

struct DepthSortMeshFunc
{

    bool operator()(Mesh* node1, Mesh * node2)
    {
        if(node1->IsTransparent())
            return false;

        else if(node2->IsTransparent())
            return true;

        else
            return (node1->GetMatrix().GetPos() - camPos) > (node2->GetMatrix().GetPos() - camPos);
    }

    Vector3f camPos;
};

void MeshParallelScene::Clear()
{
    for(unsigned i = 0; i < m_nodes.size(); i++)
        if(!m_nodes[i]->HasParent())
            if(!m_nodes[i]->IsLockPtr())
                delete m_nodes[i], m_nodes[i] = NULL;

    m_nodes.clear();
}

void MeshParallelScene::Render()
{
    Frustum* frustum = m_sceneManager->GetFrustum();

    m_frustumCullingCount = 0;

    static DepthSortMeshFunc sortFunc;

    sortFunc.camPos = m_sceneManager->GetCurCamera()->GetPos();

    std::sort(m_nodes.begin(), m_nodes.end(), sortFunc);

    for(unsigned i = 0; i < m_nodes.size(); i++)
    {
        Node* it = m_nodes[i];

        if(!it->HasParent())
            it->Process();

        if(m_enableFrustumTest && !frustum->IsInside(it))
        {
            m_frustumCullingCount++;
            continue;
        }

        it->Render();
    }
}

void MeshParallelScene::RegisterMesh(Mesh* mesh)
{
    if(std::find(m_nodes.begin(), m_nodes.end(), mesh) != m_nodes.end())
        throw Exception("MeshParallelScene::RegisterMesh; child already exist");

    mesh->SetParallelScene(this);

    m_nodes.push_back(mesh);
}

void MeshParallelScene::UnRegisterMesh(Mesh* mesh, bool deleteptr)
{
    Mesh::Array::iterator it = std::find(m_nodes.begin(), m_nodes.end(), mesh);

    if(it == m_nodes.end())
        throw Exception("MeshParallelScene::UnRegisterMesh; cannot found child");

    if(deleteptr)
        delete (*it);

    m_nodes.erase(it);
}

bool MeshParallelScene::FindFloor(Vector3f& pos)
{
    bool atleastone = false;

    pos.y = getSceneAabb().min.y - 1;

    for(unsigned i = 0; i < m_nodes.size(); i++)
    {
        float lastY = pos.y;

        if(m_nodes[i]->FindGlobalFloor(pos))
            atleastone = true;

        pos.y = std::max(pos.y, lastY);
    }

    return atleastone;
}

void MeshParallelScene::SetInFloor(Node* node)
{
    Vector3f pos = node->GetPos();

    pos.y = getSceneAabb().min.y - 1;

    for(unsigned i = 0; i < m_nodes.size(); i++)
    {
        if(node == m_nodes[i] || node->HasParent())
            continue;

        float lastY = pos.y;

        m_nodes[i]->FindGlobalFloor(pos);

        pos.y = std::max(pos.y, lastY);
    }

    node->SetPos(pos);
}

Iterator<Mesh*> MeshParallelScene::GetIterator()
{
    return Iterator<Mesh*>(m_nodes);
}

AABB MeshParallelScene::getSceneAabb()
{
    AABB sceneAabb;

    for(unsigned i = 0; i < m_nodes.size(); i++)
        sceneAabb.Count(m_nodes[i]);

    return sceneAabb;
}

void MeshParallelScene::SetEnableFrustumTest(bool enableFrustumTest)
{
    this->m_enableFrustumTest = enableFrustumTest;
}

bool MeshParallelScene::IsEnableFrustumTest() const
{
    return m_enableFrustumTest;
}

void MeshParallelScene::SetFrustumCullingCount(unsigned frustumCullingCount)
{
    this->m_frustumCullingCount = frustumCullingCount;
}

unsigned MeshParallelScene::GetFrustumCullingCount() const
{
    return m_frustumCullingCount;
}
