#include "Editbox.h"
#include "Skin.h"

using namespace std;
using namespace tbe;
using namespace tbe::gui;

#define META_COUNT (3)
#define META_UNIT (1.0f/META_COUNT)

EditBox::EditBox()
{
    m_state = NO_EVENT;
}

EditBox::EditBox(std::string label, Pencil font, Texture background)
{
    m_state = NO_EVENT;

    setLabel(label);
    setPencil(font);
    setBackground(background);
}

EditBox::~EditBox()
{
}

void EditBox::objectRender()
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

    if(m_pencil && !m_label.empty())
    {
        std::string showStr = m_pencil.wrapeLine(m_label, m_size.x - (8 * m_size.x / 100));

        if(m_activate)
            showStr += ']';

        m_pencil.display(m_pencil.centerOf(showStr, m_pos, m_size), showStr);
    }
}

bool EditBox::onEvent(const EventManager& event)
{
    if(m_activate)
    {
        if(event.notify == EventManager::EVENT_KEY_DOWN)
        {
            int key = event.lastActiveKey.first;

            if(key == EventManager::KEY_RETURN || key == EventManager::KEY_ESCAPE)
            {
                m_state = NO_EVENT;
                m_activate = false;
            }

            else if(key == EventManager::KEY_BACKSPACE || key == EventManager::KEY_DELETE)
            {
                if(!m_label.empty())
                    m_label.erase(m_label.length() - 1);
            }

            else if(key > 0 && key < 255)
            {
                m_label += event.lastActiveKey.first;
            }
        }

        else if(event.notify == EventManager::EVENT_KEY_UP)
            m_key = 0;
    }

    else if(Vector2f(event.mousePos).isInsinde(m_pos, m_size))
    {
        if(event.notify == EventManager::EVENT_MOUSE_MOVE)
            m_state = OVER;

        else if(event.notify == EventManager::EVENT_MOUSE_UP
                && event.lastActiveMouse.first == EventManager::MOUSE_BUTTON_LEFT)
        {
            m_state = EDIT;
            m_activate = true;
            return true;
        }

    }

    else
        m_state = NO_EVENT;

    return false;
}

void EditBox::setSkin(const GuiSkin& skin)
{
    setPencil(skin.pencile);
    setBackground(skin.editBox);
    setSize(skin.editBoxSize);
}
