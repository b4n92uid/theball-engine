/*
 * File:   MapMark.cpp
 * Author: b4n92uid
 *
 * Created on 9 juin 2011, 13:59
 */

#include "MapMark.h"
#include "Tools.h"

using namespace tbe;
using namespace scene;

MapMark::MapMark()
{
}

MapMark::MapMark(const MapMark& orig)
{
    copy(orig);
}

MapMark::~MapMark()
{
}

MapMark& MapMark::operator=(const MapMark& orig)
{
    copy(orig);
    return *this;
}

void MapMark::copy(const MapMark& m)
{
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
