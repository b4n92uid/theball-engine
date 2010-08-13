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

    bool operator()(Node* node1, Node * node2)
    {
        if(FarToNear)
            return(node1->GetMatrix().GetPos() - camPos) > (node2->GetMatrix().GetPos() - camPos);
        else
            return(node1->GetMatrix().GetPos() - camPos) < (node2->GetMatrix().GetPos() - camPos);
    }

    Vector3f camPos;
    bool FarToNear;
};

void MeshParallelScene::Render()
{
    typedef std::list<Mesh*> MeshList;

    Frustum* frustum = m_sceneManager->GetFrustum();

    m_frustumCullingCount = 0;

    // Préparation au tri

    MeshList opacNode, blendNode;

    for(Mesh::Map::iterator it = m_nodes.begin(); it != m_nodes.end(); it++)
    {
        if(!it->second->IsEnable())
            continue;

        if(m_enableFrustumTest && !frustum->IsInside(it->second))
        {
            m_frustumCullingCount++;
            continue;
        }

        if(it->second->IsTransparent())
            blendNode.push_back(it->second);
        else
            opacNode.push_back(it->second);
    }

    // Tri

    static DepthSortMeshFunc sortFunc;

    sortFunc.camPos = m_sceneManager->GetCurCamera()->GetPos();

    if(!blendNode.empty())
    {
        sortFunc.FarToNear = true;
        blendNode.sort(sortFunc);
    }

    if(!opacNode.empty())
    {
        sortFunc.FarToNear = false;
        opacNode.sort(sortFunc);
    }

    // Rendue

    for(MeshList::iterator itt = opacNode.begin(); itt != opacNode.end(); itt++)
        (*itt)->Render();

    for(MeshList::iterator itt = blendNode.begin(); itt != blendNode.end(); itt++)
        (*itt)->Render();
}

void MeshParallelScene::Clear()
{
    for(Mesh::Map::iterator itt = m_nodes.begin(); itt != m_nodes.end(); itt++)
        delete itt->second;

    m_nodes.clear();
}

void MeshParallelScene::AddMesh(std::string name, Mesh* node)
{
    if(!node)
        throw Exception("MeshParallelScene::AddMesh; Try to add a NULL ptr node");

    if(name.empty())
        name = tools::NameGen(m_nodes);

    else if(m_nodes.find(name) != m_nodes.end())
        throw Exception("MeshParallelScene::AddMesh; Name already exist (%s)", name.c_str());

    node->SetName(name);
    node->SetParallelScene(this);

    m_nodes[name] = node;
}

Mesh* MeshParallelScene::GetMesh(std::string name)
{
    if(m_nodes.find(name) != m_nodes.end())
        return m_nodes[name];

    else
        throw tbe::Exception("MeshParallelScene::GetMesh; Mesh not found (%s)", name.c_str());
}

Mesh* MeshParallelScene::ReleaseMesh(std::string name)
{
    if(m_nodes.find(name) == m_nodes.end())
        throw Exception("MeshParallelScene::RealeaseMesh; Mesh not found (%s)", name.c_str());

    Mesh * releaseMesh = m_nodes[name];

    m_nodes.erase(name);

    return releaseMesh;
}

void MeshParallelScene::DeleteMesh(std::string name)
{
    if(m_nodes.find(name) == m_nodes.end())
        throw Exception("MeshParallelScene::DeleteMesh; Mesh not found (%s)", name.c_str());

    delete m_nodes[name];
    m_nodes.erase(name);
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
