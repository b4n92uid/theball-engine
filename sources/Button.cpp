#include "Button.h"
#include "Skin.h"

using namespace tbe;
using namespace tbe::gui;

Button::Button()
{
    m_state = NO_EVENT;
}

Button::Button(std::string label, Pencil font, Texture background)
{
    m_state = NO_EVENT;

    setLabel(label);
    setPencil(font);
    setBackground(background);
}

Button::~Button()
{

}

void Button::setSkin(const GuiSkin& skin)
{
    setMetaCount(skin.buttonMetaCount);
    setPencil(skin.pencil);
    setBackground(skin.button);
    setSize(skin.buttonSize);
}

void Button::objectRender()
{
    const float metaUnit = 1.0 / m_metaCount;

    if(m_background)
    {
        m_background.use();

        switch(m_state)
        {
            case NO_EVENT: drawSurface(m_pos, m_size, Vector2f(0, 0), Vector2f(1, metaUnit));
                break;
            case OVER: drawSurface(m_pos, m_size, Vector2f(0, metaUnit), Vector2f(1, metaUnit * 2));
                break;
            case PUSH: drawSurface(m_pos, m_size, Vector2f(0, metaUnit * 2), Vector2f(1, metaUnit * 3));
                break;
        }
    }

    if(m_pencil)
    {
        Vector2f pos;

        if(m_textAlign & HCENTER)
            pos.x = m_pencil.centerOf(m_label, m_pos, m_size).x;
        else if(m_textAlign & LEFT)
            pos.x = m_pos.x + m_textPadding.x;
        else if(m_textAlign & RIGHT)
            pos.x = m_pos.x + m_size.x - m_pencil.sizeOf(m_label).x - m_textPadding.w;

        if(m_textAlign & VCENTER)
            pos.y = m_pencil.centerOf(m_label, m_pos, m_size).y;
        else if(m_textAlign & TOP)
            pos.y = m_pos.y + m_size.y - m_pencil.sizeOf(m_label).y - m_textPadding.h;
        else if(m_textAlign & BOTTOM)
            pos.y = m_pos.y + m_textPadding.y;

        m_pencil.display(pos, m_label);
    }
}

bool Button::onEvent(const EventManager& event)
{
    m_activate = false;

    if(Vector2f(event.mousePos).isInsinde(m_pos, m_size))
    {
        if(event.notify == EventManager::EVENT_MOUSE_DOWN && event.lastActiveMouse.first == EventManager::MOUSE_BUTTON_LEFT)
        {
            m_state = PUSH;
            return true;
        }

        else if(event.notify == EventManager::EVENT_MOUSE_UP && event.lastActiveMouse.first == EventManager::MOUSE_BUTTON_LEFT)
        {
            m_state = OVER;
            m_activate = true;
            return true;
        }

        else
            m_state = OVER;

    }

    else
        m_state = NO_EVENT;

    return false;
}
