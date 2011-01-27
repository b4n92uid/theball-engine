/* 
 * File:   TextBox.cpp
 * Author: b4n92uid
 * 
 * Created on 10 mai 2010, 19:59
 */

#include "TextBox.h"
#include "Control.h"

using namespace std;
using namespace tbe;
using namespace tbe::gui;

TextBox::TextBox()
{
    m_offsetMax = 0;
    m_offsetLine = 0;
    m_definedSize = false;
    m_textAlign = CENTER;
}

TextBox::TextBox(std::string path, int size)
{
    m_offsetMax = 0;
    m_offsetLine = 0;
    m_definedSize = false;

    m_pencil.Load(path, size);
}

TextBox::~TextBox()
{
}

void TextBox::ObjectRender()
{
    if(m_enableBackground && m_background)
    {
        m_background.Use(true);

        DrawSurface(m_pos, m_size, 0, 1);

        m_background.Use(false);
    }

    Vector2f pos = m_pencil.CenterOf(m_textDisplay, m_pos, m_size);

    switch(m_textAlign)
    {
        case LEFT:
            pos.x = m_pos.x + m_backgroundPadding.x;
            break;

        case CENTER:
            break;
    }

    if(m_definedSize)
    {
        string backupfornt = m_textDisplay.front(),
                backupback = m_textDisplay.back();

        if(m_offsetLine > 0)
            m_textDisplay.back() = "<<<";

        if(m_offsetLine < m_offsetMax)
            m_textDisplay.front() = ">>>";

        m_pencil.Display(pos, m_textDisplay);

        if(m_offsetLine > 0)
            m_textDisplay.back() = backupback;

        if(m_offsetLine < m_offsetMax)
            m_textDisplay.front() = backupfornt;
    }

    else
        m_pencil.Display(pos, m_textDisplay);
}

void TextBox::Write(GuiString text)
{
    m_text = text;

    PrepareDisplay();
}

void TextBox::PrepareDisplay()
{
    if(m_definedSize)
    {
        unsigned displayLines = unsigned((m_size.y - m_backgroundPadding.y * 2.0) / (m_pencil.GetFontSize() + m_pencil.GetLineSpace()));

        vector<string> buffer;

        for(unsigned i = 0; i < m_text.size(); i++)
        {
            const string& curline = m_text[i];

            vector<string> widthOverflow = m_pencil.GetWrapedLines(curline, m_size.x - m_backgroundPadding.x * 2.0f);
            buffer.insert(buffer.end(), widthOverflow.begin(), widthOverflow.end());
        }

        if(displayLines > m_text.size())
            m_offsetMax = min(buffer.size() - displayLines, (unsigned)0);
        else
            m_offsetMax = max(buffer.size() - displayLines, (unsigned)0);

        vector<string>::iterator start = max(buffer.begin(), buffer.begin() + m_offsetLine);
        vector<string>::iterator end = min(buffer.end(), buffer.begin() + m_offsetLine + displayLines);

        m_textDisplay.assign(start, end);
    }

    else
    {
        m_size = m_pencil.SizeOf(m_text) + m_backgroundPadding;
        m_textDisplay.assign(m_text.begin(), m_text.end());
    }

    reverse(m_textDisplay.begin(), m_textDisplay.end());
}

void TextBox::SetDefinedSize(bool definedSize)
{
    this->m_definedSize = definedSize;
}

bool TextBox::IsDefinedSize() const
{
    return m_definedSize;
}

void TextBox::SetTextAlign(TextAlign textAlign)
{
    this->m_textAlign = textAlign;
}

TextBox::TextAlign TextBox::GetTextAlign() const
{
    return m_textAlign;
}

bool TextBox::OnEvent(const EventManager& event)
{
    Vector2f mousePos = event.mousePos;

    if(m_definedSize && mousePos.IsInsinde(m_pos, m_size))
    {
        if(event.notify == EventManager::EVENT_MOUSE_DOWN)
        {
            if((event.lastActiveMouse.first == EventManager::MOUSE_BUTTON_LEFT ||
               event.lastActiveMouse.first == EventManager::MOUSE_BUTTON_WHEEL_DOWN)
               && m_offsetLine < m_offsetMax)
                m_offsetLine++;

            if((event.lastActiveMouse.first == EventManager::MOUSE_BUTTON_RIGHT ||
               event.lastActiveMouse.first == EventManager::MOUSE_BUTTON_WHEEL_UP)
               && m_offsetLine > 0)
                m_offsetLine--;
        }

        PrepareDisplay();
    }

    return false;
}

void TextBox::SetSkin(const GuiSkin& skin)
{
    SetPencil(skin.pencile);
}

void TextBox::SetOffsetLine(unsigned offsetLine)
{
    this->m_offsetLine = offsetLine;
}

unsigned TextBox::GetOffsetLine() const
{
    return m_offsetLine;
}

void TextBox::SetBackgroundPadding(Vector2f backgroundPadding)
{
    this->m_backgroundPadding = backgroundPadding;
}

Vector2f TextBox::GetBackgroundPadding() const
{
    return m_backgroundPadding;
}
