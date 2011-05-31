/*
 * File:   ListBox.cpp
 * Author: b4n92uid
 *
 * Created on 15 juin 2010, 21:41
 */

#include "ListBox.h"

using namespace std;
using namespace tbe;
using namespace tbe::gui;

ListBox::ListBox()
{
    m_lay.setOrientation(Layout::Vertical);
    m_lay.setSpace(4);

    m_offset = 0;
    m_offsetMax = 0;
    m_backgroundPadding = 0;
    m_definedSize = false;

    m_currentItem = NULL;
}

ListBox::~ListBox()
{
    clear();
}

ListBox& ListBox::push(const std::string& label)
{
    Item* it = new Item(label);
    it->setPencil(m_pencil);
    it->setSize(m_pencil.sizeOf(label));

    m_totalItems.push_back(it);

    return *this;
}

ListBox& ListBox::push(const std::string& label, const Any& data)
{
    Item* it = new Item(label, data);
    it->setPencil(m_pencil);
    it->setSize(m_pencil.sizeOf(label));

    m_totalItems.push_back(it);

    return *this;
}

void ListBox::cancelSelection()
{
    if(!m_currentItem)
        return;

    m_currentItem->setSelected(false);
    m_currentItem = NULL;
}

bool ListBox::isSelection()
{
    return m_currentItem;
}

void ListBox::setDefinedSize(bool definedSize)
{
    this->m_definedSize = definedSize;
}

bool ListBox::isDefinedSize() const
{
    return m_definedSize;
}

void ListBox::setCurrentString(const std::string& str)
{
    m_currentItem->setLabel(str);
}

std::string ListBox::getCurrentString()
{
    return m_currentItem->getLabel();
}

void ListBox::setCurrentIndex(unsigned index)
{
    m_currentItem = m_totalItems[index];
}

unsigned ListBox::getCurrentIndex()
{
    return find(m_totalItems.begin(), m_totalItems.end(), m_currentItem) - m_totalItems.begin();
}

void ListBox::setCurrentData(const Any& data)
{
    m_currentItem->setData(data);
}

Any ListBox::getCurrentData()
{
    return m_currentItem->getData();
}

void ListBox::setString(unsigned index, const std::string& str)
{
    m_totalItems[index]->setLabel(str);
}

std::string ListBox::getString(unsigned index)
{
    return m_totalItems[index]->getLabel();
}

void ListBox::setData(unsigned index, const Any& data)
{
    m_totalItems[index]->setData(data);
}

Any ListBox::getData(unsigned index)
{
    return m_totalItems[index]->getData();
}

unsigned ListBox::getCount()
{
    return m_totalItems.size();
}

void ListBox::clear()
{
    for(unsigned i = 0; i < m_totalItems.size(); i++)
        delete m_totalItems[i];

    m_totalItems.clear();

    for(unsigned i = 0; i < m_displayItems.size(); i++)
        delete m_displayItems[i];

    m_displayItems.clear();
}

void ListBox::setSkin(const GuiSkin& gui)
{
    setPencil(gui.pencile);

    for(unsigned i = 0; i < m_totalItems.size(); i++)
        m_totalItems[i]->setPencil(m_pencil);
}

void ListBox::setBackgroundPadding(Vector2f backgroundPadding)
{
    this->m_backgroundPadding = backgroundPadding;

    m_lay.setBorder(m_backgroundPadding);
}

Vector2f ListBox::getBackgroundPadding() const
{
    return m_backgroundPadding;
}

bool ListBox::onEvent(const EventManager& event)
{
    m_activate = false;

    if(!Vector2f(event.mousePos).isInsinde(m_pos, m_size))
        return false;

    if(event.notify != EventManager::EVENT_MOUSE_DOWN)
        return false;

    switch(event.lastActiveMouse.first)
    {
        case EventManager::MOUSE_BUTTON_LEFT:
            for(unsigned i = 0; i < m_displayItems.size(); i++)
                if(m_displayItems[i]->onEvent(event))
                {
                    m_currentItem = m_displayItems[i];

                    for(unsigned j = 0; j < m_displayItems.size(); j++)
                        m_totalItems[j]->setSelected(false);

                    m_currentItem->setSelected(true);

                    return m_activate = true;
                }
            break;

        case EventManager::MOUSE_BUTTON_WHEEL_UP:
            if(m_offset < m_offsetMax)
                m_offset++;

            update();
            break;

        case EventManager::MOUSE_BUTTON_WHEEL_DOWN:
            if(m_offset > 0)
                m_offset--;

            update();
            break;
    }

    return true;
}

void ListBox::update()
{
    for(unsigned i = 0; i < m_displayItems.size(); i++)
        delete m_displayItems[i];

    m_displayItems.clear();
    m_lay.clear();

    if(m_definedSize)
    {
        unsigned displayLines = unsigned(
                (m_size.y - m_backgroundPadding.y * 2.0f)
                / (m_pencil.getFontSize() + m_lay.getSpace())
                );

        for(unsigned i = m_offset; i < m_totalItems.size(); i++)
        {
            if(m_displayItems.size() >= displayLines)
                break;

            m_displayItems.push_back(new Item(*m_totalItems[i]));
            m_lay.addControl(m_displayItems.back());

            string label = m_displayItems.back()->getLabel();
            label = m_pencil.wrapeLine(label, m_size.x - m_backgroundPadding.x * 2.0f, true);
            m_displayItems.back()->setLabel(label);
        }

        if(displayLines > m_displayItems.size())
            m_offsetMax = min(m_totalItems.size() - displayLines, (unsigned)0);
        else
            m_offsetMax = max(m_totalItems.size() - displayLines, (unsigned)0);

        if(m_offset > 0)
            m_displayItems.front()->setLabel("<<<");

        if(m_offset < m_offsetMax)
            m_displayItems.back()->setLabel(">>>");
    }

    else
    {
        for(unsigned i = 0; i < m_totalItems.size(); i++)
        {
            m_displayItems.push_back(new Item(*m_totalItems[i]));
            m_lay.addControl(m_displayItems.back());
        }

        m_lay.update();

        m_size = m_lay.getSize()
                + m_backgroundPadding * 2.0f
                + m_lay.getBorder() * Vector2f(0, 1);
    }

    reverse(m_displayItems.begin(), m_displayItems.end());
}

void ListBox::objectRender()
{
    drawBackground();

    m_lay.setPos(m_pos);
    m_lay.update();

    for(unsigned i = 0; i < m_displayItems.size(); i++)
        m_displayItems[i]->render();
}

ListBox::Item::Item(std::string label)
{
    m_label = label;
    m_selected = false;
}

ListBox::Item::Item(std::string label, const Any& data)
{
    m_label = label;
    m_selected = false;
    m_data = data;
}

ListBox::Item::~Item()
{

}

void ListBox::Item::setData(const Any& data)
{
    this->m_data = data;
}

Any ListBox::Item::getData() const
{
    return m_data;
}

void ListBox::Item::setSelected(bool selected)
{
    this->m_selected = selected;
    m_pencil.setColor(selected ? Vector4f(0, 0, 1, 1) : Vector4f(1));
}

bool ListBox::Item::isSelected() const
{
    return m_selected;
}

bool ListBox::Item::onEvent(const EventManager& event)
{
    return Vector2f(event.mousePos).isInsinde(m_pos, m_size);
}

void ListBox::Item::objectRender()
{
    m_pencil.display(m_pos, m_label);
}
