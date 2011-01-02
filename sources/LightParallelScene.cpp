/* 
 * File:   LightParallelScene.cpp
 * Author: b4n92uid
 * 
 * Created on 17 décembre 2010, 16:54
 */

#include "LightParallelScene.h"
#include "Light.h"

using namespace tbe;
using namespace scene;

LightParallelScene::LightParallelScene()
{
}

LightParallelScene::~LightParallelScene()
{
}

void LightParallelScene::Render()
{
    // Mise a jour des lumieres
    while(!m_rendredNodes.empty())
    {
        m_rendredNodes.front()->Render();
        m_rendredNodes.pop_front();
    }
}
