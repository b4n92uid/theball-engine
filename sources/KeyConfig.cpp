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

    SetPencil(font);
    SetBackground(background);
}

KeyConfig::~KeyConfig()
{
}

void KeyConfig::ObjectRender()
{
    m_background.Use();

    switch(m_state)
    {
        case NO_EVENT: DrawSurface(m_pos, m_size, Vector2f(0, 0), Vector2f(1, META_UNIT));
            break;
        case OVER: DrawSurface(m_pos, m_size, Vector2f(0, META_UNIT), Vector2f(1, META_UNIT * 2));
            break;
        case EDIT: DrawSurface(m_pos, m_size, Vector2f(0, META_UNIT * 2), Vector2f(1, META_UNIT * 3));
            break;
    }

    if(m_pencil)
    {
        m_pencil.Display(m_pencil.CenterOf(m_label, m_pos, m_size), m_label);
    }
}

void KeyConfig::SetSkin(const GuiSkin& skin)
{
    SetPencil(skin.pencile);
    SetBackground(skin.editBox);
    SetSize(skin.editBoxSize);
}

bool KeyConfig::OnEvent(const EventManager& event)
{
    if(m_activate)
    {
        if(event.notify == EventManager::EVENT_KEY_DOWN)
        {
            SetKeyCode(event.lastActiveKey.first, event.lastActiveKey.second);
            m_activate = false;
            m_state = NO_EVENT;
            return true;
        }

        else if(event.notify == EventManager::EVENT_MOUSE_DOWN)
        {
            SetMouseCode(event.lastActiveMouse.first, event.lastActiveMouse.second);
            m_activate = false;
            m_state = NO_EVENT;
            return true;
        }
    }

    else if(event.notify == EventManager::EVENT_MOUSE_MOVE)
    {
        if(Vector2f(event.mousePos).IsInsinde(m_pos, m_size))
            m_state = OVER;

        else
            m_state = NO_EVENT;
    }

    else if(event.notify == EventManager::EVENT_MOUSE_DOWN
            && event.lastActiveMouse.first == EventManager::MOUSE_BUTTON_LEFT)
    {
        if(Vector2f(event.mousePos).IsInsinde(m_pos, m_size))
        {
            m_state = EDIT;
            m_activate = true;
            return true;
        }
    }

    return false;
}

void KeyConfig::SetKeyCode(int keyCode, std::string keyName)
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

int KeyConfig::GetKeyCode() const
{
    return m_keyCode;
}

void KeyConfig::SetMouseCode(int mouseCode, std::string buttonName)
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

int KeyConfig::GetMouseCode() const
{
    return m_mouseCode;
}
