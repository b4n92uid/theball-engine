/*
 * File:   Fog.cpp
 * Author: b4n92uid
 *
 * Created on 23 janvier 2010, 14:33
 */

#include "Fog.h"

using namespace tbe;
using namespace tbe::scene;

Fog::Fog()
{
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glHint(GL_FOG_HINT, GL_DONT_CARE);

    clear();
}

Fog::~Fog()
{
    setEnable(false);
}

void Fog::clear()
{
    setColor(1.0f);
    setDensity(0.1f);
    setStart(8.0f);
    setEnd(64.0f);
    setEnable(false);
}

void Fog::setEnd(float end)
{
    this->m_end = end;
    glFogf(GL_FOG_END, m_end);
}

float Fog::getEnd() const
{
    return m_end;
}

void Fog::setStart(float start)
{
    this->m_start = start;
    glFogf(GL_FOG_START, m_start);
}

float Fog::getStart() const
{
    return m_start;
}

void Fog::setDensity(float density)
{
    this->m_density = density;
    glFogf(GL_FOG_DENSITY, m_density);
}

float Fog::getDensity() const
{
    return m_density;
}

void Fog::setColor(Vector4f color)
{
    this->m_color = color;
    glFogfv(GL_FOG_COLOR, m_color);
}

Vector4f Fog::getColor() const
{
    return m_color;
}

void Fog::setEnable(bool enable)
{
    this->m_enable = enable;

    m_enable ? glEnable(GL_FOG) : glDisable(GL_FOG);
}

bool Fog::isEnable() const
{
    return m_enable;
}

