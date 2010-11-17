#include "Gauge.h"
#include "Skin.h"

#define META_COUNT (2)
#define META_UNIT (1.0f/META_COUNT)

using namespace tbe;
using namespace tbe::gui;
using namespace std;

Gauge::Gauge()
{
    m_value = 0;

    m_smooth = false;
    m_smoothSpeed = 0;

    m_valueTarget = 0;
}

Gauge::Gauge(std::string label, Pencil font, Texture background)
{
    SetLabel(label);
    SetPencil(font);
    SetBackground(background);

    m_value = 0;

    m_smooth = false;
    m_smoothSpeed = 0;

    m_valueTarget = 0;
}

Gauge::~Gauge()
{

}

bool Gauge::OnEvent(const EventManager& event)
{
    return false;
}

void Gauge::ObjectRender()
{
    glBindTexture(GL_TEXTURE_2D, m_background);

    DrawSurface(m_pos, m_size, 0, Vector2f(1, META_UNIT));

    if(m_smooth)
    {
        if(m_valueTarget > m_value)
            m_value = min(m_value + m_smoothSpeed, m_valueTarget);

        if(m_valueTarget < m_value)
            m_value = max(m_value - m_smoothSpeed, m_valueTarget);
    }

    DrawSurface(
                m_pos,
                Vector2f(m_value * m_size.x / 100.0f, m_size.y),
                Vector2f(0, META_UNIT),
                Vector2f(m_value / 100.0f, META_UNIT * 2)
                );

    if(m_pencil)
    {
        m_pencil.Display(m_pencil.CenterOf(m_label, m_pos, m_size), m_label);
    }
}

void Gauge::SetValue(int value, bool rel)
{
    #ifdef TBE_COMPILE_DEBUG
    if(value < 0 && !rel)
        cout << "Gauge::SetValue; Warning : value < 0 && rel == false" << endl;
    #endif

    if(m_smooth)
    {
        if(rel)
            m_valueTarget += value;

        else
            m_valueTarget = value;

        if(m_valueTarget > 100) m_valueTarget = 100;
        if(m_valueTarget < 0) m_valueTarget = 0;
    }

    else
    {
        if(rel)
            m_value += value;

        else
            m_value = value;

        if(m_value > 100) m_value = 100;

        if(m_value < 0) m_value = 0;
    }
}

int Gauge::GetValue()
{

    return m_value;
}

void Gauge::SetSkin(const GuiSkin& skin)
{
    SetPencil(skin.pencile);
    SetBackground(skin.gauge);
    SetSize(skin.gaugeSize);
}

void Gauge::SetSmooth(bool b, int speed)
{
    m_smooth = b;

    #ifdef TBE_COMPILE_DEBUG
    if(b && speed < 1)
        cout << "Gauge::SetSmooth; nWarning : speed < 1" << endl;
    #endif

    m_smoothSpeed = speed;
}

bool Gauge::IsSmooth()
{
    return m_smoothSpeed;
}
