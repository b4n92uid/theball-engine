#ifndef GAUGE_H
#define GAUGE_H

#include "Texture.h"

#include "Control.h"
#include "Pencil.h"

namespace tbe
{
namespace gui
{

/// \brief Bar de progression

class Gauge : public Control
{
public:
    Gauge();
    Gauge(std::string label, Pencil font, Texture background);
    ~Gauge();

    bool onEvent(const EventManager& event);
    void setSkin(const GuiSkin& skin);

    // Valuer
    void setValue(int value, bool rel = false);
    int getValue();

    // Effet lisser
    void setSmooth(bool b, int speed = 1);
    bool isSmooth();

    void setReverse(bool reverse);
    bool isReverse() const;

protected:
    void objectRender();

    int m_value;
    int m_valueTarget;
    int m_smoothSpeed;
    bool m_smooth;
    bool m_reverse;
};

}
}

#endif
