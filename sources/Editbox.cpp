#include "Editbox.h"
#include "Skin.h"

using namespace std;
using namespace tbe;
using namespace tbe::gui;

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
    const float metaUnit = 1.0 / m_metaCount;

    m_background.use();

    switch(m_state)
    {
        case NO_EVENT: drawSurface(m_pos, m_size, Vector2f(0, 0), Vector2f(1, metaUnit));
            break;
        case OVER: drawSurface(m_pos, m_size, Vector2f(0, metaUnit), Vector2f(1, metaUnit * 2));
            break;
        case EDIT: drawSurface(m_pos, m_size, Vector2f(0, metaUnit * 2), Vector2f(1, metaUnit * 3));
            break;
    }

    if(m_pencil && !m_label.empty())
    {
        std::string showStr = m_pencil.wrapeLine(m_label, m_size.x - (m_textPadding.x * 2));

        if(m_activate)
            showStr += ']';

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

        m_pencil.display(pos, showStr);
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
    setMetaCount(skin.editBoxMetaCount);
    setPencil(skin.pencil);
    setBackground(skin.editBox);
    setSize(skin.editBoxSize);
}
