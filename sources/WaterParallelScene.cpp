/* 
 * File:   WaterParallelScene.cpp
 * Author: b4n92uid
 * 
 * Created on 21 mai 2010, 22:31
 */

#include "WaterParallelScene.h"
#include "Exception.h"
#include "Tools.h"

using namespace tbe;
using namespace tbe::scene;

WaterParallelScene::WaterParallelScene()
{
}

WaterParallelScene::~WaterParallelScene()
{
}

void WaterParallelScene::Render()
{
    if(!m_enable)
        return;

    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    for(Node* it = m_nodes.front(); it <= m_nodes.back(); it++)
    {
        it->Render();
    }

    glPopAttrib();
}
