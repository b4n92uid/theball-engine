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

    for(ParticlesEmiter::Map::iterator i = m_nodes.begin(); i != m_nodes.end(); i++)
    {
        if(i->second->IsTopLevel())
            i->second->Process();

        i->second->Render();
    }

    ParticlesEmiter::EndSpiritDraw();
}

void ParticlesParallelScene::Clear()
{

}

void ParticlesParallelScene::AddParticlesEmiter(std::string name, ParticlesEmiter* node)
{
    if(name.empty())
        name = tools::NameGen(m_nodes);

    else if(m_nodes.find(name) != m_nodes.end())
        throw Exception("SceneManager::AddParticlesEmiter; Name already exist (%s)", name.c_str());

    if(!node)
        throw Exception("SceneManager::AddParticlesEmiter; Try to add a NULL ptr node");

    node->SetParallelScene(this);

    m_nodes[name] = node;
}

ParticlesEmiter* ParticlesParallelScene::GetParticlesEmiter(std::string name)
{
    if(m_nodes.find(name) != m_nodes.end())
        return m_nodes[name];

    else
        throw tbe::Exception("ParticlesEmiterParallelScene::GetParticlesEmiter; ParticlesEmiter not found (%s)", name.c_str());
}

ParticlesEmiter* ParticlesParallelScene::ReleaseParticlesEmiter(std::string name)
{
    if(m_nodes.find(name) == m_nodes.end())
        throw Exception("ParticlesEmiterParallelScene::RealeaseParticlesEmiter; ParticlesEmiter not found (%s)", name.c_str());

    ParticlesEmiter * releaseParticlesEmiter = m_nodes[name];

    m_nodes.erase(name);

    return releaseParticlesEmiter;
}

void ParticlesParallelScene::ReleaseParticlesEmiter(ParticlesEmiter* node)
{
    for(ParticlesEmiter::Map::iterator i = m_nodes.begin(); i != m_nodes.end(); i++)
        if(i->second == node)
        {
            ReleaseParticlesEmiter(i->first);
            return;
        }
}

void ParticlesParallelScene::DeleteParticlesEmiter(ParticlesEmiter* node)
{
    for(ParticlesEmiter::Map::iterator i = m_nodes.begin(); i != m_nodes.end(); i++)
        if(i->second == node)
        {
            DeleteParticlesEmiter(i->first);
            return;
        }
}

void ParticlesParallelScene::DeleteParticlesEmiter(std::string name)
{
    if(m_nodes.find(name) == m_nodes.end())
        throw Exception("ParticlesEmiterParallelScene::DeleteParticlesEmiter; ParticlesEmiter not found (%s)", name.c_str());

    delete m_nodes[name];
    m_nodes.erase(name);
}
