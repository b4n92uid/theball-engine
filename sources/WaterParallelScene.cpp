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
#include "Water.h"

using namespace tbe;
using namespace tbe::scene;

WaterParallelScene::WaterParallelScene()
{
    m_inPreRender = false;
}

WaterParallelScene::~WaterParallelScene()
{
}

void WaterParallelScene::PreRender()
{
    m_inPreRender = true;

    for(unsigned i = 0; i < m_nodes.size(); i++)
    {
        Water* water = m_nodes[i];

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
        if(m_nodes[i]->IsAttached())
            m_nodes[i]->Render();

    glPopAttrib();
}
