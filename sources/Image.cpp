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
        SetBackground(background);
        SetSize(background.GetSize());
    }
}

bool Image::OnEvent(const EventManager& event)
{
    return false;
}

void Image::ObjectRender()
{
    m_background.Use();

    switch(m_drawDiriction)
    {
    case DRAW_UPPER_LEFT:
        DrawSurface(m_pos, m_size, 0, 1);
        break;
    case DRAW_UPPER_RIGHT:
        DrawSurface(Vector2f(m_pos.x, m_pos.y + m_size.y), Vector2f(m_size.x, -m_size.y), 0, 1);
        break;
    }
}

void Image::SetSkin(const GuiSkin& skin)
{
}

void Image::SetDrawDiriction(DrawDiriction drawDiriction)
{
    this->m_drawDiriction = drawDiriction;
}

Image::DrawDiriction Image::GetDrawDiriction() const
{
    return m_drawDiriction;
}
