#include "Gauge.h"
#include "Skin.h"

using namespace tbe;
using namespace tbe::gui;
using namespace std;

Gauge::Gauge()
{
    m_value = 0;

    m_smooth = false;
    m_smoothSpeed = 0;

    m_valueTarget = 0;

    m_metaCount = 2;

    m_reverse = false;
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

    m_metaCount = 2;

    m_reverse = false;
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
    const float metaUnit = 1.0 / m_metaCount;

    m_background.use();

    drawSurface(m_pos, m_size, 0, Vector2f(1, metaUnit));

    if(m_smooth)
    {
        if(m_valueTarget > m_value)
            m_value = min(m_value + m_smoothSpeed, m_valueTarget);

        if(m_valueTarget < m_value)
            m_value = max(m_value - m_smoothSpeed, m_valueTarget);
    }

    float length = m_value * m_size.x / 100.0f;

    if(m_reverse)
        drawSurface(Vector2f(m_pos.x + m_size.x - length, m_pos.y),
                    Vector2f(length, m_size.y),
                    Vector2f(m_value / 100.0f, metaUnit),
                    Vector2f(1, metaUnit * 2));
    else
        drawSurface(m_pos,
                    Vector2f(m_value * m_size.x / 100.0f, m_size.y),
                    Vector2f(0, metaUnit),
                    Vector2f(m_value / 100.0f, metaUnit * 2));

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
    setMetaCount(skin.gaugeMetaCount);
    setPencil(skin.pencil);
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

void Gauge::setReverse(bool reverse)
{
    this->m_reverse = reverse;
}

bool Gauge::isReverse() const
{
    return m_reverse;
}
