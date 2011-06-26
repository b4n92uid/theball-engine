/*
 * File:   MapMark.cpp
 * Author: b4n92uid
 *
 * Created on 9 juin 2011, 13:59
 */

#include "MapMark.h"
#include "Exception.h"
#include "MapMarkParallelScene.h"
#include "Tools.h"

using namespace tbe;
using namespace scene;

MapMark::MapMark(MapMarkParallelScene* parallelScene)
{
    Node::m_parallelScene = m_parallelScene = parallelScene;

    m_sceneManager = m_parallelScene->getSceneManager();

    m_parallelScene->registerNode(this);
}

MapMark::MapMark(const MapMark& orig) : Node(orig)
{
    copy(orig);

    m_parallelScene->registerNode(this);
}

MapMark::~MapMark()
{
    m_parallelScene->unregisterNode(this);
}

MapMark& MapMark::operator=(const MapMark& orig)
{
    Node::operator=(orig);

    return copy(orig);
}

MapMark& MapMark::copy(const MapMark& orig)
{
    Node::m_parallelScene = m_parallelScene = orig.m_parallelScene;

    m_sceneManager = m_parallelScene->getSceneManager();

    return *this;
}

MapMark* MapMark::clone()
{
    return new MapMark(*this);
}

void MapMark::process()
{
    if(!m_enable)
        return;

    for_each(m_childs.begin(), m_childs.end(), std::mem_fun(&Node::process));
}

void MapMark::render()
{
    // Nothging to do...
}

Node::CtorMap MapMark::constructionMap(std::string root)
{
    Node::CtorMap ctormap = Node::constructionMap(root);

    ctormap["class"] = "MapMark";

    return ctormap;
}
