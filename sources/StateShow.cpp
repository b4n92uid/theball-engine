/* 
 * File:   StateShow.cpp
 * Author: Administrateur
 * 
 * Created on 11 septembre 2009, 21:40
 */

#include "StateShow.h"

using namespace tbe;
using namespace tbe::gui;

StateShow::StateShow()
{
    m_stateCount = 1;
    m_curState = 0;
}

StateShow::~StateShow()
{
}

bool StateShow::onEvent(const EventManager& event)
{
    m_activate = false;

    if(Vector2f(event.mousePos).isInsinde(m_pos, m_size))
    {
        if(event.notify == EventManager::EVENT_MOUSE_DOWN
                && event.lastActiveMouse.first == EventManager::MOUSE_BUTTON_LEFT)
        {
            if(m_curState++ >= m_stateCount)
                m_curState = 0;

            m_activate = true;
            return true;
        }

        if(event.notify == EventManager::EVENT_MOUSE_DOWN
                && event.lastActiveMouse.first == EventManager::MOUSE_BUTTON_RIGHT)
        {
            if(m_curState-- <= 0)
                m_curState = m_stateCount - 1;

            m_activate = true;
            return true;
        }
    }

    return false;
}

void StateShow::objectRender()
{
    m_background.use();
    drawSurface(m_pos, m_size, Vector2f(0, 1.0f / m_stateCount * m_curState), Vector2f(1, 1.0f / m_stateCount * (m_curState + 1)));
}

void StateShow::setCurState(unsigned curState)
{
    this->m_curState = curState;
}

void StateShow::setStateCount(unsigned stateCount)
{
    this->m_stateCount = stateCount;
}

unsigned StateShow::getStateCount() const
{
    return m_stateCount;
}

unsigned StateShow::getCurState() const
{
    return m_curState;
}

void StateShow::setSkin(const GuiSkin& skin)
{
    setSize(skin.stateShowSize);
}
