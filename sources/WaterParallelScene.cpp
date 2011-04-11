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

void WaterParallelScene::preRender()
{
    m_inPreRender = true;

    for(unsigned i = 0; i < m_nodes.size(); i++)
    {
        Water* water = m_nodes[i];

        // Render Reflection
        water->beginReflection();
        m_sceneManager->render(false);
        water->endReflection();

        // Render Refraction
        water->beginRefraction();
        m_sceneManager->render(false);
        water->endRefraction();
    }

    m_inPreRender = false;
}

void WaterParallelScene::render()
{
    if(!m_enable || m_inPreRender)
        return;

    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    for(unsigned i = 0; i < m_nodes.size(); i++)
        if(m_nodes[i]->isAttached())
            m_nodes[i]->render();

    glPopAttrib();
}
