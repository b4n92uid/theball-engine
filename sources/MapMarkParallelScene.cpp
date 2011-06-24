/*
 * File:   MapMarkParallelScene.cpp
 * Author: b4n92uid
 *
 * Created on 14 juin 2011, 23:31
 */

#include "MapMarkParallelScene.h"
#include "MapMark.h"

using namespace tbe;
using namespace scene;

MapMarkParallelScene::MapMarkParallelScene()
{
}

MapMarkParallelScene::~MapMarkParallelScene()
{
}

void MapMarkParallelScene::render()
{
}

MapMark* MapMarkParallelScene::findMark(std::string name)
{
    for(unsigned i = 0; i < m_nodes.size(); i++)
        if(m_nodes[i]->getName() == name)
            return m_nodes[i];

    return NULL;
}
