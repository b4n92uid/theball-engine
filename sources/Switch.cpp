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

Switch::Switch(Pencil font, Texture background)
{
    m_state = DEAD;

    setBackground(background);
    setPencil(font);
    setupRender();
}

Switch::Switch()
{
    m_state = DEAD;

    setupRender();
}

Switch::~Switch()
{

}

bool Switch::onEvent(const EventManager& event)
{
    bool inLeft = Vector2f(event.mousePos).isInsinde(m_pos, Vector2f(m_size.x / 2, m_size.y));
    bool inRight = Vector2f(event.mousePos).isInsinde(Vector2f(m_pos.x + m_size.x / 2, m_pos.y), Vector2f(m_size.x / 2, m_size.y));

    if(Vector2f(event.mousePos).isInsinde(m_pos, m_size))
    {
        if(event.notify == EventManager::EVENT_MOUSE_DOWN)
        {
            if(event.lastActiveMouse.first == EventManager::MOUSE_BUTTON_WHEEL_UP)
                increment();

            else if(event.lastActiveMouse.first == EventManager::MOUSE_BUTTON_WHEEL_DOWN)
                decrement();
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
            decrement();
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
            increment();
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

void Switch::setSkin(const GuiSkin& skin)
{
    setMetaCount(skin.switchBoxMetaCount);
    setBackground(skin.switchBox);
    setSize(skin.switchBoxSize);
    setPencil(skin.pencil);
    setupRender();
}

void Switch::setupRender()
{
    const float metaUnit = 1.0 / m_metaCount;

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
        Vector2f(0, metaUnit),
        Vector2f(0.5, 0),
        Vector2f(0.5, metaUnit),
        Vector2f(0.5, 0),
        Vector2f(0.5, metaUnit),
        Vector2f(1, 0),
        Vector2f(1, metaUnit)
    };
    Vector2f stOverLeft[8] = {
        Vector2f(0, metaUnit),
        Vector2f(0, metaUnit * 2),
        Vector2f(0.5, metaUnit),
        Vector2f(0.5, metaUnit * 2),
        Vector2f(0.5, 0),
        Vector2f(0.5, metaUnit),
        Vector2f(1, 0),
        Vector2f(1, metaUnit)
    };
    Vector2f stOverRight[8] = {
        Vector2f(0, 0),
        Vector2f(0, metaUnit),
        Vector2f(0.5, 0),
        Vector2f(0.5, metaUnit),
        Vector2f(0.5, metaUnit),
        Vector2f(0.5, metaUnit * 2),
        Vector2f(1, metaUnit),
        Vector2f(1, metaUnit * 2)
    };
    Vector2f stPushLeft[8] = {
        Vector2f(0, metaUnit * 2),
        Vector2f(0, metaUnit * 3),
        Vector2f(0.5, metaUnit * 2),
        Vector2f(0.5, metaUnit * 3),
        Vector2f(0.5, 0),
        Vector2f(0.5, metaUnit),
        Vector2f(1, 0),
        Vector2f(1, metaUnit)
    };
    Vector2f stPushRight[8] = {
        Vector2f(0, 0),
        Vector2f(0, metaUnit),
        Vector2f(0.5, 0),
        Vector2f(0.5, metaUnit),
        Vector2f(0.5, metaUnit * 2),
        Vector2f(0.5, metaUnit * 3),
        Vector2f(1, metaUnit * 2),
        Vector2f(1, metaUnit * 3)
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

void Switch::objectRender()
{
    m_background.use();

    switch(m_state)
    {
        case DEAD: drawSurface(switchVertex.pos, switchVertex.stOut, 8);
            break;
        case OVER_LEFT: drawSurface(switchVertex.pos, switchVertex.stOverLeft, 8);
            break;
        case OVER_RIGHT: drawSurface(switchVertex.pos, switchVertex.stOverRight, 8);
            break;
        case PUSH_LEFT: drawSurface(switchVertex.pos, switchVertex.stPushLeft, 8);
            break;
        case PUSH_RIGHT: drawSurface(switchVertex.pos, switchVertex.stPushRight, 8);
            break;
    }

    if(m_pencil)
    {
        m_pencil.display(m_pencil.centerOf(m_label, m_pos, m_size), m_label);
    }
}

SwitchString::SwitchString()
{
    m_it = m_content.end();
}

void SwitchString::setCurrent(unsigned index)
{
    index = min(index, m_content.size() - 1);

    m_it = m_content.begin() + index;
    m_label = m_it->str;
}

unsigned SwitchString::getCurrent()
{
    return m_it - m_content.begin();
}

void SwitchString::setString(const std::string& str)
{
    m_it->str = str;
}

std::string SwitchString::getString() const
{
    return m_it->str;
}

void SwitchString::setData(const Any& data)
{
    m_it->data = data;
}

Any SwitchString::getData() const
{
    return m_it->data;
}

void SwitchString::setString(unsigned index, const std::string& str)
{
    m_content[index].str = str;
}

std::string SwitchString::getString(unsigned index) const
{
    return m_content[index].str;
}

void SwitchString::setData(unsigned index, const Any& data)
{
    m_content[index].data = data;
}

Any SwitchString::getData(unsigned index) const
{
    return m_content[index].data;
}

void SwitchString::setData(std::string str, const Any& data)
{
    for(unsigned i = 0; i < m_content.size(); i++)
        if(m_content[i].str == str)
        {
            m_content[i].data = data;
            break;
        }
}

Any SwitchString::getData(std::string str) const
{
    for(unsigned i = 0; i < m_content.size(); i++)
        if(m_content[i].str == str)
            return m_content[i].data;

    return Any();
}

SwitchString& SwitchString::push(std::string str)
{
    Entry e;
    e.str = str;
    m_content.push_back(e);

    m_it = --m_content.end();
    m_label = m_it->str;

    return *this;
}

SwitchString& SwitchString::push(std::string str, const Any& data)
{
    Entry e;
    e.str = str;
    e.data = data;
    m_content.push_back(e);

    m_it = --m_content.end();

    m_label = m_it->str;

    return *this;
}

unsigned SwitchString::getCount() const
{
    return m_content.size();
}

void SwitchString::deleteAll()
{
    m_content.clear();
    m_it = m_content.end();
}

void SwitchString::deleteOn(unsigned index)
{
    m_content.erase(m_content.begin() + index);

    increment();
}

void SwitchString::deleteOn(std::string str)
{
    for(unsigned i = 0; i < m_content.size(); i++)
        if(m_content[i].str == str)
        {
            m_content.erase(m_content.begin() + i);
            break;
        }

    increment();
}

void SwitchString::increment()
{
    if(m_content.empty())
    {
        m_it = m_content.end();
        m_label.clear();
        return;
    }

    ++m_it;

    if(m_it >= m_content.end())
        m_it = m_content.begin();

    m_label = m_it->str;
}

void SwitchString::decrement()
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
