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
    m_lay.SetOrientation(Layout::Vertical);
    m_lay.SetSpace(4);

    m_offset = 0;
    m_offsetMax = 0;
    m_backgroundPadding = 0;
    m_definedSize = false;

    m_currentItem = NULL;
}

ListBox::~ListBox()
{
    Clear();
}

ListBox& ListBox::Push(const std::string& label)
{
    Item* it = new Item(label);
    it->SetPencil(m_pencil);
    it->SetSize(m_pencil.SizeOf(label));

    m_totalItems.push_back(it);

    return *this;
}

ListBox& ListBox::Push(const std::string& label, const Any& data)
{
    Item* it = new Item(label, data);
    it->SetPencil(m_pencil);
    it->SetSize(m_pencil.SizeOf(label));

    m_totalItems.push_back(it);

    return *this;
}

void ListBox::CancelSelection()
{
    if(!m_currentItem)
        return;

    m_currentItem->SetSelected(false);
    m_currentItem = NULL;
}

bool ListBox::IsSelection()
{
    return m_currentItem;
}

void ListBox::SetDefinedSize(bool definedSize)
{
    this->m_definedSize = definedSize;
}

bool ListBox::IsDefinedSize() const
{
    return m_definedSize;
}

void ListBox::SetCurrentString(const std::string& str)
{
    m_currentItem->SetLabel(str);
}

std::string ListBox::GetCurrentString()
{
    return m_currentItem->GetLabel();
}

void ListBox::SetCurrentIndex(unsigned index)
{
    m_currentItem = m_totalItems[index];
}

unsigned ListBox::GetCurrentIndex()
{
    return find(m_totalItems.begin(), m_totalItems.end(), m_currentItem) - m_totalItems.begin();
}

void ListBox::SetCurrentData(const Any& data)
{
    m_currentItem->SetData(data);
}

Any ListBox::GetCurrentData()
{
    return m_currentItem->GetData();
}

void ListBox::SetString(unsigned index, const std::string& str)
{
    m_totalItems[index]->SetLabel(str);
}

std::string ListBox::GetString(unsigned index)
{
    return m_totalItems[index]->GetLabel();
}

void ListBox::SetData(unsigned index, const Any& data)
{
    m_totalItems[index]->SetData(data);
}

Any ListBox::GetData(unsigned index)
{
    return m_totalItems[index]->GetData();
}

unsigned ListBox::GetCount()
{
    return m_totalItems.size();
}

void ListBox::Clear()
{
    for(unsigned i = 0; i < m_totalItems.size(); i++)
        delete m_totalItems[i];

    m_totalItems.clear();

    for(unsigned i = 0; i < m_displayItems.size(); i++)
        delete m_displayItems[i];

    m_displayItems.clear();
}

void ListBox::SetSkin(const GuiSkin& gui)
{
    SetPencil(gui.pencile);

    for(unsigned i = 0; i < m_totalItems.size(); i++)
        m_totalItems[i]->SetPencil(m_pencil);
}

void ListBox::SetBackgroundPadding(Vector2f backgroundPadding)
{
    this->m_backgroundPadding = backgroundPadding;

    m_lay.SetBorder(m_backgroundPadding);
}

Vector2f ListBox::GetBackgroundPadding() const
{
    return m_backgroundPadding;
}

bool ListBox::OnEvent(const EventManager& event)
{
    m_activate = false;

    if(!Vector2f(event.mousePos).IsInsinde(m_pos, m_size))
        return false;

    if(event.notify != EventManager::EVENT_MOUSE_DOWN)
        return false;

    switch(event.lastActiveMouse.first)
    {
        case EventManager::MOUSE_BUTTON_LEFT:
            for(unsigned i = 0; i < m_displayItems.size(); i++)
                if(m_displayItems[i]->OnEvent(event))
                {
                    m_currentItem = m_displayItems[i];

                    for(unsigned j = 0; j < m_displayItems.size(); j++)
                        m_totalItems[j]->SetSelected(false);

                    m_currentItem->SetSelected(true);

                    return m_activate = true;
                }
            break;

        case EventManager::MOUSE_BUTTON_WHEEL_UP:
            if(m_offset < m_offsetMax)
                m_offset++;

            Update();
            break;

        case EventManager::MOUSE_BUTTON_WHEEL_DOWN:
            if(m_offset > 0)
                m_offset--;

            Update();
            break;
    }

    return true;
}

void ListBox::Update()
{
    for(unsigned i = 0; i < m_displayItems.size(); i++)
        delete m_displayItems[i];

    m_displayItems.clear();
    m_lay.Clear();

    if(m_definedSize)
    {
        unsigned displayLines = unsigned(
                (m_size.y - m_backgroundPadding.y * 2.0f)
                / (m_pencil.GetFontSize() + m_lay.GetSpace())
                );

        for(unsigned i = m_offset; i < m_totalItems.size(); i++)
        {
            if(m_displayItems.size() >= displayLines)
                break;

            m_displayItems.push_back(new Item(*m_totalItems[i]));
            m_lay.AddControl(m_displayItems.back());

            string label = m_displayItems.back()->GetLabel();
            label = m_pencil.WrapeLine(label, m_size.x - m_backgroundPadding.x * 2.0f, true);
            m_displayItems.back()->SetLabel(label);
        }

        if(displayLines > m_displayItems.size())
            m_offsetMax = min(m_totalItems.size() - displayLines, (unsigned)0);
        else
            m_offsetMax = max(m_totalItems.size() - displayLines, (unsigned)0);

        if(m_offset > 0)
            m_displayItems.front()->SetLabel("<<<");

        if(m_offset < m_offsetMax)
            m_displayItems.back()->SetLabel(">>>");
    }

    else
    {
        for(unsigned i = 0; i < m_totalItems.size(); i++)
        {
            m_displayItems.push_back(new Item(*m_totalItems[i]));
            m_lay.AddControl(m_displayItems.back());
        }

        m_lay.Update();

        m_size = m_lay.GetSize()
                + m_backgroundPadding * 2.0f
                + m_lay.GetBorder() * Vector2f(0, 1);
    }

    reverse(m_displayItems.begin(), m_displayItems.end());
}

void ListBox::ObjectRender()
{
    if(m_enableBackground && m_background)
    {
        m_background.Use(true);
        DrawSurface(m_pos, m_size, 0, 1);
        m_background.Use(false);
    }

    m_lay.SetPos(m_pos);
    m_lay.Update();

    for(unsigned i = 0; i < m_displayItems.size(); i++)
        m_displayItems[i]->Render();
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

void ListBox::Item::SetData(const Any& data)
{
    this->m_data = data;
}

Any ListBox::Item::GetData() const
{
    return m_data;
}

void ListBox::Item::SetSelected(bool selected)
{
    this->m_selected = selected;
    m_pencil.SetColor(selected ? Vector4f(0, 0, 1, 1) : Vector4f(1));
}

bool ListBox::Item::IsSelected() const
{
    return m_selected;
}

bool ListBox::Item::OnEvent(const EventManager& event)
{
    return Vector2f(event.mousePos).IsInsinde(m_pos, m_size);
}

void ListBox::Item::ObjectRender()
{
    m_pencil.Display(m_pos, m_label);
}
