/* 
 * File:   WaterParallelScene.cpp
 * Author: b4n92uid
 * 
 * Created on 21 mai 2010, 22:31
 */

#include "WaterParallelScene.h"
#include "SceneManager.h"
#include "Exception.h"
#include "Tools.h"

using namespace tbe;
using namespace tbe::scene;

WaterParallelScene::WaterParallelScene()
{
    m_inPreRender = false;
}

WaterParallelScene::~WaterParallelScene()
{
    Clear();
}

void WaterParallelScene::Clear()
{
    for(unsigned i = 0; i < m_nodes.size(); i++)
        if(!m_nodes[i]->HasParent())
            if(!m_nodes[i]->IsLockPtr())
                delete m_nodes[i], m_nodes[i] = NULL;

    m_nodes.clear();
}

void WaterParallelScene::PreRender()
{
    m_inPreRender = true;

    for(unsigned i = 0; i < m_nodes.size(); i++)
    {
        Water* water = dynamic_cast<Water*> (m_nodes[i]);

        // Render Reflection
        water->BeginReflection();
        m_sceneManager->Render(false);
        water->EndReflection();

        // Render Refraction
        water->BeginRefraction();
        m_sceneManager->Render(false);
        water->EndRefraction();
    }

    m_inPreRender = false;
}

void WaterParallelScene::Render()
{
    if(!m_enable || m_inPreRender)
        return;

    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    for(unsigned i = 0; i < m_nodes.size(); i++)
        m_nodes[i]->Render();

    glPopAttrib();
}

void WaterParallelScene::RegisterWater(Water* water)
{
    if(std::find(m_nodes.begin(), m_nodes.end(), water) != m_nodes.end())
        throw Exception("WaterParallelScene::RegisterWater; child already exist");

    water->SetParallelScene(this);

    m_nodes.push_back(water);
}

void WaterParallelScene::UnRegisterWater(Water* water, bool deleteptr)
{
    Water::Array::iterator it = std::find(m_nodes.begin(), m_nodes.end(), water);

    if(it == m_nodes.end())
        throw Exception("WaterParallelScene::UnRegisterWater; cannot found child");

    if(deleteptr)
        delete (*it);

    m_nodes.erase(it);
}

Iterator<Water*> WaterParallelScene::GetIterator()
{
    return Iterator<Water*>(m_nodes);
}
