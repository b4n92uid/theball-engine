/* 
 * File:   KeyConfig.cpp
 * Author: b4n92uid
 * 
 * Created on 5 février 2010, 11:50
 */

#include "KeyConfig.h"
#include "Skin.h"

#define META_COUNT (3)
#define META_UNIT (1.0f/META_COUNT)

using namespace tbe;
using namespace tbe::gui;

KeyConfig::KeyConfig()
{
    m_state = NO_EVENT;
    m_keyCode = -1;
    m_mouseCode = -1;
}

KeyConfig::KeyConfig(Pencil font, Texture background)
{
    m_state = NO_EVENT;
    m_keyCode = -1;
    m_mouseCode = -1;

    setPencil(font);
    setBackground(background);
}

KeyConfig::~KeyConfig()
{
}

void KeyConfig::objectRender()
{
    m_background.use();

    switch(m_state)
    {
        case NO_EVENT: drawSurface(m_pos, m_size, Vector2f(0, 0), Vector2f(1, META_UNIT));
            break;
        case OVER: drawSurface(m_pos, m_size, Vector2f(0, META_UNIT), Vector2f(1, META_UNIT * 2));
            break;
        case EDIT: drawSurface(m_pos, m_size, Vector2f(0, META_UNIT * 2), Vector2f(1, META_UNIT * 3));
            break;
    }

    if(m_pencil)
    {
        m_pencil.display(m_pencil.centerOf(m_label, m_pos, m_size), m_label);
    }
}

void KeyConfig::setSkin(const GuiSkin& skin)
{
    setPencil(skin.pencile);
    setBackground(skin.editBox);
    setSize(skin.editBoxSize);
}

bool KeyConfig::onEvent(const EventManager& event)
{
    if(m_activate)
    {
        if(event.notify == EventManager::EVENT_KEY_DOWN)
        {
            setKeyCode(event.lastActiveKey.first, event.lastActiveKey.second);
            m_activate = false;
            m_state = NO_EVENT;
            return true;
        }

        else if(event.notify == EventManager::EVENT_MOUSE_DOWN)
        {
            setMouseCode(event.lastActiveMouse.first, event.lastActiveMouse.second);
            m_activate = false;
            m_state = NO_EVENT;
            return true;
        }
    }

    else if(event.notify == EventManager::EVENT_MOUSE_MOVE)
    {
        if(Vector2f(event.mousePos).isInsinde(m_pos, m_size))
            m_state = OVER;

        else
            m_state = NO_EVENT;
    }

    else if(event.notify == EventManager::EVENT_MOUSE_DOWN
            && event.lastActiveMouse.first == EventManager::MOUSE_BUTTON_LEFT)
    {
        if(Vector2f(event.mousePos).isInsinde(m_pos, m_size))
        {
            m_state = EDIT;
            m_activate = true;
            return true;
        }
    }

    return false;
}

void KeyConfig::setKeyCode(int keyCode, std::string keyName)
{
    this->m_keyCode = keyCode;
    this->m_mouseCode = -1;

    std::ostringstream exp;

    if(keyName.empty())
    {
        if(m_keyCode > 0 && m_keyCode < 255)
            exp << (char)m_keyCode;
        else
            exp << m_keyCode;
    }

    else
    {
        exp << keyName;
    }

    m_label = exp.str();
}

int KeyConfig::getKeyCode() const
{
    return m_keyCode;
}

void KeyConfig::setMouseCode(int mouseCode, std::string buttonName)
{
    this->m_mouseCode = mouseCode;
    this->m_keyCode = -1;

    std::ostringstream exp;

    if(buttonName.empty())
        exp << mouseCode;

    else
        exp << buttonName;

    m_label = exp.str();
}

int KeyConfig::getMouseCode() const
{
    return m_mouseCode;
}
