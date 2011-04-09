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
    setLabel(label);
    setPencil(font);
    setBackground(background);

    m_value = 0;

    m_smooth = false;
    m_smoothSpeed = 0;

    m_valueTarget = 0;
}

Gauge::~Gauge()
{

}

bool Gauge::onEvent(const EventManager& event)
{
    return false;
}

void Gauge::objectRender()
{
    glBindTexture(GL_TEXTURE_2D, m_background);

    drawSurface(m_pos, m_size, 0, Vector2f(1, META_UNIT));

    if(m_smooth)
    {
        if(m_valueTarget > m_value)
            m_value = min(m_value + m_smoothSpeed, m_valueTarget);

        if(m_valueTarget < m_value)
            m_value = max(m_value - m_smoothSpeed, m_valueTarget);
    }

    drawSurface(
                m_pos,
                Vector2f(m_value * m_size.x / 100.0f, m_size.y),
                Vector2f(0, META_UNIT),
                Vector2f(m_value / 100.0f, META_UNIT * 2)
                );

    if(m_pencil)
    {
        m_pencil.display(m_pencil.centerOf(m_label, m_pos, m_size), m_label);
    }
}

void Gauge::setValue(int value, bool rel)
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

int Gauge::getValue()
{

    return m_value;
}

void Gauge::setSkin(const GuiSkin& skin)
{
    setPencil(skin.pencile);
    setBackground(skin.gauge);
    setSize(skin.gaugeSize);
}

void Gauge::setSmooth(bool b, int speed)
{
    m_smooth = b;

    #ifdef TBE_COMPILE_DEBUG
    if(b && speed < 1)
        cout << "Gauge::SetSmooth; nWarning : speed < 1" << endl;
    #endif

    m_smoothSpeed = speed;
}

bool Gauge::isSmooth()
{
    return m_smoothSpeed;
}
