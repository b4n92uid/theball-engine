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
    m_type = Box;
    m_size = 1;
    m_color = 1;
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
    m_color = m.m_color;
    m_size = m.m_size;
    m_type = m.m_type;
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

void MapMark::setColor(Vector3f color)
{
    this->m_color = color;
}

Vector3f MapMark::getColor() const
{
    return m_color;
}

void MapMark::setSize(float size)
{
    this->m_size = size;
}

float MapMark::getSize() const
{
    return m_size;
}

void MapMark::setType(Type type)
{
    this->m_type = type;
}

MapMark::Type MapMark::getType() const
{
    return m_type;
}

Node::CtorMap MapMark::constructionMap(std::string root)
{
    Node::CtorMap ctormap = Node::constructionMap(root);

    ctormap["class"] = "MapMark";
    ctormap["color"] = tools::vec3ToStr(m_color);
    ctormap["type"] = tools::numToStr(m_type);
    ctormap["size"] = tools::numToStr(m_size);

    return ctormap;
}
