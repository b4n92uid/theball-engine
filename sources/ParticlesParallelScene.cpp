/* 
 * File:   ParticlesParallelScene.cpp
 * Author: b4n92uid
 * 
 * Created on 30 avril 2010, 23:09
 */

#include "ParticlesParallelScene.h"
#include "SceneManager.h"
#include "Particles.h"

#include "Tools.h"

using namespace tbe;
using namespace tbe::scene;

ParticlesParallelScene::ParticlesParallelScene()
{
}

ParticlesParallelScene::~ParticlesParallelScene()
{
}

void ParticlesParallelScene::Render()
{
    ParticlesEmiter::BeginSpiritDraw(m_sceneManager->GetFovy(), m_sceneManager->GetViewport().y);

    for(unsigned i = 0; i < m_nodes.size(); i++)
    {
        Node* it = m_nodes[i];

        if(!it->HasParent())
            it->Process();

        it->Render();
    }

    ParticlesEmiter::EndSpiritDraw();
}
