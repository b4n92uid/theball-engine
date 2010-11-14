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
        if(dynamic_cast<Mesh*>(node1)->IsTransparent())
            return false;

        else
            return(node1->GetMatrix().GetPos() - camPos) > (node2->GetMatrix().GetPos() - camPos);
    }

    Vector3f camPos;
};

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
