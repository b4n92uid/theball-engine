#include "Image.h"

using namespace tbe;
using namespace tbe::gui;

Image::Image()
{
    m_drawDiriction = DRAW_UPPER_LEFT;
}

Image::Image(Texture background)
{
    m_drawDiriction = DRAW_UPPER_LEFT;

    if(background)
    {
        setBackground(background);
        setSize(background.getSize());
    }
}

bool Image::onEvent(const EventManager& event)
{
    return false;
}

void Image::objectRender()
{
    m_background.use();

    switch(m_drawDiriction)
    {
    case DRAW_UPPER_LEFT:
        drawSurface(m_pos, m_size, 0, 1);
        break;
    case DRAW_UPPER_RIGHT:
        drawSurface(Vector2f(m_pos.x, m_pos.y + m_size.y), Vector2f(m_size.x, -m_size.y), 0, 1);
        break;
    }
}

void Image::setSkin(const GuiSkin& skin)
{
}

void Image::setDrawDiriction(DrawDiriction drawDiriction)
{
    this->m_drawDiriction = drawDiriction;
}

Image::DrawDiriction Image::getDrawDiriction() const
{
    return m_drawDiriction;
}
