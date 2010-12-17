/* 
 * File:   LightParallelScene.cpp
 * Author: b4n92uid
 * 
 * Created on 17 décembre 2010, 16:54
 */

#include "LightParallelScene.h"

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
    for(unsigned i = 0; i < m_nodes.size(); i++)
        m_nodes[i]->Render();
}
