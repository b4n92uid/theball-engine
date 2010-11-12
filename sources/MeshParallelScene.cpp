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
    typedef std::list<Node*> MeshList;

    Frustum* frustum = m_sceneManager->GetFrustum();

    m_frustumCullingCount = 0;

    // Préparation au tri

    MeshList opacNode, blendNode;

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

        if(dynamic_cast<Mesh*>(it)->IsTransparent())
            blendNode.push_back(it);
        else
            opacNode.push_back(it);
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
