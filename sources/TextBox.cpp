/*
 * File:   TextBox.cpp
 * Author: b4n92uid
 *
 * Created on 10 mai 2010, 19:59
 */

#include "TextBox.h"
#include "Control.h"
#include "Vector2.h"

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

    m_pencil.load(path, size);
}

TextBox::~TextBox()
{
}

void TextBox::objectRender()
{
    drawBackground();

    Vector2f pos = m_pencil.centerOf(m_textDisplay, m_pos, m_size);

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

        if(m_arrowTexture)
        {
            m_arrowTexture.use(true);

            Vector2f size = m_arrowTexture.getSize() / Vector2f(1, 2);
            Vector2f arrdn = m_pos + m_backgroundPadding;
            Vector2f arrup = m_pos + Vector2f(m_backgroundPadding.x, m_size.y - m_backgroundPadding.y - size.y);

            if(m_offsetLine > 0)
            {
                m_textDisplay.back().clear();
                drawSurface(arrup, size, 0, Vector2f(1, 0.5));
            }

            if(m_offsetLine < m_offsetMax)
            {
                m_textDisplay.front().clear();
                drawSurface(arrdn, size, Vector2f(0, 0.5), Vector2f(1, 1));
            }

            m_arrowTexture.use(false);
        }
        else
        {
            if(m_offsetLine > 0)
                m_textDisplay.front() = "<<<";

            if(m_offsetLine < m_offsetMax)
                m_textDisplay.front() = ">>>";
        }

        m_pencil.display(pos, m_textDisplay);

        if(m_offsetLine > 0)
            m_textDisplay.back() = backupback;

        if(m_offsetLine < m_offsetMax)
            m_textDisplay.front() = backupfornt;
    }

    else
        m_pencil.display(pos, m_textDisplay);
}

void TextBox::write(GuiString text)
{
    m_text = text;

    prepareDisplay();
}

void TextBox::prepareDisplay()
{
    if(m_definedSize)
    {
        unsigned displayLines = unsigned((m_size.y - m_backgroundPadding.y * 2.0) / (m_pencil.getFontSize() + m_pencil.getLineSpace()));

        vector<string> buffer;

        for(unsigned i = 0; i < m_text.size(); i++)
        {
            const string& curline = m_text[i];

            vector<string> widthOverflow = m_pencil.getWrapedLines(curline, m_size.x - m_backgroundPadding.x * 2.0f);
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
        m_size = m_pencil.sizeOf(m_text) + m_backgroundPadding;
        m_textDisplay.assign(m_text.begin(), m_text.end());
    }

    reverse(m_textDisplay.begin(), m_textDisplay.end());
}

void TextBox::setDefinedSize(bool definedSize)
{
    this->m_definedSize = definedSize;
}

bool TextBox::isDefinedSize() const
{
    return m_definedSize;
}

void TextBox::setTextAlign(TextAlign textAlign)
{
    this->m_textAlign = textAlign;
}

TextBox::TextAlign TextBox::getTextAlign() const
{
    return m_textAlign;
}

void TextBox::setArrowTexture(Texture arrowTexture)
{
    this->m_arrowTexture = arrowTexture;
}

Texture TextBox::getArrowTexture() const
{
    return m_arrowTexture;
}

bool TextBox::onEvent(const EventManager& event)
{
    Vector2f mousePos = event.mousePos;

    if(m_definedSize && mousePos.isInsinde(m_pos, m_size))
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

        prepareDisplay();
    }

    return false;
}

void TextBox::setSkin(const GuiSkin& skin)
{
    setPencil(skin.pencile);
}

void TextBox::setOffsetLine(unsigned offsetLine)
{
    this->m_offsetLine = offsetLine;
}

unsigned TextBox::getOffsetLine() const
{
    return m_offsetLine;
}

void TextBox::setBackgroundPadding(Vector2f backgroundPadding)
{
    this->m_backgroundPadding = backgroundPadding;
}

Vector2f TextBox::getBackgroundPadding() const
{
    return m_backgroundPadding;
}
