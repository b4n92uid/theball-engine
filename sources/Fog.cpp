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

    Clear();
}

Fog::~Fog()
{
    SetEnable(false);
}

void Fog::Clear()
{
    SetColor(1.0f);
    SetDensity(0.1f);
    SetStart(2.0f);
    SetEnd(32.0f);
    SetEnable(false);
}

void Fog::SetEnd(float end)
{
    this->m_end = end;
    glFogf(GL_FOG_END, m_end);
}

float Fog::GetEnd() const
{
    return m_end;
}

void Fog::SetStart(float start)
{
    this->m_start = start;
    glFogf(GL_FOG_START, m_start);
}

float Fog::GetStart() const
{
    return m_start;
}

void Fog::SetDensity(float density)
{
    this->m_density = density;
    glFogf(GL_FOG_DENSITY, m_density);
}

float Fog::GetDensity() const
{
    return m_density;
}

void Fog::SetColor(Vector4f color)
{
    this->m_color = color;
    glFogfv(GL_FOG_COLOR, m_color);
}

Vector4f Fog::GetColor() const
{
    return m_color;
}

void Fog::SetEnable(bool enable)
{
    this->m_enable = enable;

    m_enable ? glEnable(GL_FOG) : glDisable(GL_FOG);
}

bool Fog::IsEnable() const
{
    return m_enable;
}

