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
    Clear();
}

void WaterParallelScene::Render()
{
    if(!m_enable)
        return;

    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    for(Water::Map::iterator itt = m_nodes.begin(); itt != m_nodes.end(); itt++)
    {
        itt->second->Render();
    }

    glPopAttrib();
}

void WaterParallelScene::Clear()
{
    for(Water::Map::iterator itt = m_nodes.begin(); itt != m_nodes.end(); itt++)
        delete itt->second;

    m_nodes.clear();
}

void WaterParallelScene::AddWater(std::string name, Water* node)
{
    if(!node)
        throw Exception("WaterParallelScene::AddWater; Try to add a NULL ptr node");

    if(name.empty())
        name = tools::NameGen(m_nodes);

    else if(m_nodes.find(name) != m_nodes.end())
        throw Exception("WaterParallelScene::AddWater; Name already exist (%s)", name.c_str());

    node->SetParallelScene(this);

    m_nodes[name] = node;
}
