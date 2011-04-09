#include "Button.h"
#include "Skin.h"

#define META_COUNT 3.0f
#define META_UNIT (1.0f/META_COUNT)

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
    setPencil(skin.pencile);
    setBackground(skin.button);
    setSize(skin.buttonSize);
}

void Button::objectRender()
{
    if(m_background)
    {
        m_background.use();

        switch(m_state)
        {
        case NO_EVENT: drawSurface(m_pos, m_size, Vector2f(0, 0), Vector2f(1, META_UNIT));
            break;
        case OVER: drawSurface(m_pos, m_size, Vector2f(0, META_UNIT), Vector2f(1, META_UNIT * 2));
            break;
        case PUSH: drawSurface(m_pos, m_size, Vector2f(0, META_UNIT * 2), Vector2f(1, META_UNIT * 3));
            break;
        }
    }

    if(m_pencil)
    {
        m_pencil.display(m_pencil.centerOf(m_label, m_pos, m_size), m_label);
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
