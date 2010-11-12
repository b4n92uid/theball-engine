#include "Switch.h"
#include "Skin.h"

using namespace std;
using namespace tbe;
using namespace tbe::gui;

static struct
{
    Vector2f pos[8];
    Vector2f stOut[8];
    Vector2f stOverLeft[8];
    Vector2f stOverRight[8];
    Vector2f stPushLeft[8];
    Vector2f stPushRight[8];

} switchVertex;

#define META_COUNT 3.0f
#define META_UNIT (1.0f/META_COUNT)

Switch::Switch(Pencil font, Texture background)
{
    m_state = DEAD;

    SetBackground(background);
    SetPencil(font);
    SetupRender();
}

Switch::Switch()
{
    m_state = DEAD;

    SetupRender();
}

Switch::~Switch()
{

}

bool Switch::OnEvent(const EventManager& event)
{
    bool inLeft = Vector2f(event.mousePos).IsInsinde(m_pos, Vector2f(m_size.x / 2, m_size.y));
    bool inRight = Vector2f(event.mousePos).IsInsinde(Vector2f(m_pos.x + m_size.x / 2, m_pos.y), Vector2f(m_size.x / 2, m_size.y));

    if(Vector2f(event.mousePos).IsInsinde(m_pos, m_size))
    {
        if(event.notify == EventManager::EVENT_MOUSE_DOWN)
        {
            if(event.lastActiveMouse.first == EventManager::MOUSE_BUTTON_WHEEL_UP)
                Increment();

            else if(event.lastActiveMouse.first == EventManager::MOUSE_BUTTON_WHEEL_DOWN)
                Decrement();
        }
    }

    if(inLeft)
    {
        if(event.notify == EventManager::EVENT_MOUSE_MOVE)
        {
            m_state = OVER_LEFT;
            m_activate = false;
        }

        else if(event.notify == EventManager::EVENT_MOUSE_DOWN)
        {
            m_state = PUSH_LEFT;
            m_activate = true;
            return true;
        }

        else if(event.notify == EventManager::EVENT_MOUSE_UP
                && event.lastActiveMouse.first == EventManager::MOUSE_BUTTON_LEFT
                && m_activate)
        {
            m_state = OVER_LEFT;
            Decrement();
            m_activate = true;
            return true;
        }

        else
            m_activate = false;
    }

    else if(inRight)
    {
        if(event.notify == EventManager::EVENT_MOUSE_MOVE)
        {
            m_state = OVER_RIGHT;
            m_activate = false;
        }

        else if(event.notify == EventManager::EVENT_MOUSE_DOWN)
        {
            m_state = PUSH_RIGHT;
            m_activate = true;
            return true;
        }

        else if(event.notify == EventManager::EVENT_MOUSE_UP
                && event.lastActiveMouse.first == EventManager::MOUSE_BUTTON_LEFT
                && m_activate)
        {
            m_state = OVER_RIGHT;
            Increment();
            m_activate = true;
            return true;
        }

        else
            m_activate = false;
    }

    else
        m_state = DEAD;

    return false;
}

void Switch::SetSkin(const GuiSkin& skin)
{
    SetBackground(skin.switchBox);
    SetSize(skin.switchBoxSize);
    SetPencil(skin.pencile);
}

void Switch::SetupRender()
{
    Vector2f pos[8] = {
        Vector2f(0, 0),
        Vector2f(0, 1),
        Vector2f(0.5, 0),
        Vector2f(0.5, 1),
        Vector2f(0.5, 0),
        Vector2f(0.5, 1),
        Vector2f(1, 0),
        Vector2f(1, 1)
    };
    Vector2f stOut[8] = {
        Vector2f(0, 0),
        Vector2f(0, META_UNIT),
        Vector2f(0.5, 0),
        Vector2f(0.5, META_UNIT),
        Vector2f(0.5, 0),
        Vector2f(0.5, META_UNIT),
        Vector2f(1, 0),
        Vector2f(1, META_UNIT)
    };
    Vector2f stOverLeft[8] = {
        Vector2f(0, META_UNIT),
        Vector2f(0, META_UNIT * 2),
        Vector2f(0.5, META_UNIT),
        Vector2f(0.5, META_UNIT * 2),
        Vector2f(0.5, 0),
        Vector2f(0.5, META_UNIT),
        Vector2f(1, 0),
        Vector2f(1, META_UNIT)
    };
    Vector2f stOverRight[8] = {
        Vector2f(0, 0),
        Vector2f(0, META_UNIT),
        Vector2f(0.5, 0),
        Vector2f(0.5, META_UNIT),
        Vector2f(0.5, META_UNIT),
        Vector2f(0.5, META_UNIT * 2),
        Vector2f(1, META_UNIT),
        Vector2f(1, META_UNIT * 2)
    };
    Vector2f stPushLeft[8] = {
        Vector2f(0, META_UNIT * 2),
        Vector2f(0, META_UNIT * 3),
        Vector2f(0.5, META_UNIT * 2),
        Vector2f(0.5, META_UNIT * 3),
        Vector2f(0.5, 0),
        Vector2f(0.5, META_UNIT),
        Vector2f(1, 0),
        Vector2f(1, META_UNIT)
    };
    Vector2f stPushRight[8] = {
        Vector2f(0, 0),
        Vector2f(0, META_UNIT),
        Vector2f(0.5, 0),
        Vector2f(0.5, META_UNIT),
        Vector2f(0.5, META_UNIT * 2),
        Vector2f(0.5, META_UNIT * 3),
        Vector2f(1, META_UNIT * 2),
        Vector2f(1, META_UNIT * 3)
    };

    for(unsigned i = 0; i < 8; i++)
    {
        switchVertex.pos[i] = pos[i];
        switchVertex.stOut[i] = stOut[i];
        switchVertex.stOverLeft[i] = stOverLeft[i];
        switchVertex.stOverRight[i] = stOverRight[i];
        switchVertex.stPushLeft[i] = stPushLeft[i];
        switchVertex.stPushRight[i] = stPushRight[i];

    }
}

void Switch::ObjectRender()
{
    m_background.Use();

    switch(m_state)
    {
        case DEAD: DrawSurface(switchVertex.pos, switchVertex.stOut, 8);
            break;
        case OVER_LEFT: DrawSurface(switchVertex.pos, switchVertex.stOverLeft, 8);
            break;
        case OVER_RIGHT: DrawSurface(switchVertex.pos, switchVertex.stOverRight, 8);
            break;
        case PUSH_LEFT: DrawSurface(switchVertex.pos, switchVertex.stPushLeft, 8);
            break;
        case PUSH_RIGHT: DrawSurface(switchVertex.pos, switchVertex.stPushRight, 8);
            break;
    }

    if(m_pencil)
    {
        m_pencil.Display(m_pencil.CenterOf(m_label, m_pos, m_size), m_label);
    }
}

SwitchString::SwitchString()
{
    m_it = m_content.end();
}

void SwitchString::SetCurrent(unsigned index)
{
    index = min(index, m_content.size() - 1);

    m_it = m_content.begin() + index;
    m_label = m_it->str;
}

unsigned SwitchString::GetCurrent()
{
    return m_it - m_content.begin();
}

void SwitchString::SetString(const std::string& str)
{
    m_it->str = str;
}

std::string SwitchString::GetString() const
{
    return m_it->str;
}

void SwitchString::SetData(const Any& data)
{
    m_it->data = data;
}

Any SwitchString::GetData() const
{
    return m_it->data;
}

void SwitchString::SetString(unsigned index, const std::string& str)
{
    m_content[index].str = str;
}

std::string SwitchString::GetString(unsigned index) const
{
    return m_content[index].str;
}

void SwitchString::SetData(unsigned index, const Any& data)
{
    m_content[index].data = data;
}

Any SwitchString::GetData(unsigned index) const
{
    return m_content[index].data;
}

void SwitchString::SetData(std::string str, const Any& data)
{
    for(unsigned i = 0; i < m_content.size(); i++)
        if(m_content[i].str == str)
        {
            m_content[i].data = data;
            break;
        }
}

Any SwitchString::GetData(std::string str) const
{
    for(unsigned i = 0; i < m_content.size(); i++)
        if(m_content[i].str == str)
            return m_content[i].data;

    return Any();
}

SwitchString& SwitchString::Push(std::string str)
{
    Entry e;
    e.str = str;
    m_content.push_back(e);

    m_it = --m_content.end();
    m_label = m_it->str;

    return *this;
}

SwitchString& SwitchString::Push(std::string str, const Any& data)
{
    Entry e;
    e.str = str;
    e.data = data;
    m_content.push_back(e);

    m_it = --m_content.end();

    m_label = m_it->str;

    return *this;
}

unsigned SwitchString::GetCount() const
{
    return m_content.size();
}

void SwitchString::DeleteAll()
{
    m_content.clear();
    m_it = m_content.end();
}

void SwitchString::Delete(unsigned index)
{
    m_content.erase(m_content.begin() + index);

    Increment();
}

void SwitchString::Delete(std::string str)
{
    for(unsigned i = 0; i < m_content.size(); i++)
        if(m_content[i].str == str)
        {
            m_content.erase(m_content.begin() + i);
            break;
        }

    Increment();
}

void SwitchString::Increment()
{
    if(m_content.empty())
    {
        m_it = m_content.end();
        m_label.clear();
        return;
    }

    m_it++;

    if(m_it >= m_content.end())
        m_it = m_content.begin();

    m_label = m_it->str;
}

void SwitchString::Decrement()
{
    if(m_content.empty())
    {
        m_it = m_content.end();
        m_label.clear();
        return;
    }

    m_it--;

    if(m_it <= --m_content.begin())
        m_it = --m_content.end();

    m_label = m_it->str;
}
