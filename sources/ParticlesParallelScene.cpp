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

    while(!m_rendredNodes.empty())
    {
        m_rendredNodes.front()->Render();
        m_rendredNodes.pop_front();
    }

    ParticlesEmiter::EndSpiritDraw();
}
