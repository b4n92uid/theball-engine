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
    Clear();
}

void ParticlesParallelScene::Clear()
{
    for(unsigned i = 0; i < m_nodes.size(); i++)
        if(!m_nodes[i]->HasParent())
            if(!m_nodes[i]->IsLockPtr())
                delete m_nodes[i], m_nodes[i] = NULL;

    m_nodes.clear();
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

void ParticlesParallelScene::RegisterParticles(ParticlesEmiter* pemiter)
{
    if(std::find(m_nodes.begin(), m_nodes.end(), pemiter) != m_nodes.end())
        throw Exception("ParticlesParallelScene::RegisterParticles; child already exist");

    pemiter->SetParallelScene(this);

    m_nodes.push_back(pemiter);
}

void ParticlesParallelScene::UnRegisterParticles(ParticlesEmiter* pemiter, bool deleteptr)
{
    ParticlesEmiter::Array::iterator it = std::find(m_nodes.begin(), m_nodes.end(), pemiter);

    if(it == m_nodes.end())
        throw Exception("ParticlesParallelScene::UnRegisterParticles; cannot found child");

    if(deleteptr)
        delete (*it);

    m_nodes.erase(it);
}

Iterator<ParticlesEmiter*> ParticlesParallelScene::GetIterator()
{
    return Iterator<ParticlesEmiter*>(m_nodes);
}
