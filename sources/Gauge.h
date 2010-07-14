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

    bool OnEvent(const EventManager& event);
    void SetSkin(const GuiSkin& skin);

    // Valuer
    void SetValue(int value, bool rel = false);
    int GetValue();

    // Effet lisser
    void SetSmooth(bool b, int speed = 1);
    bool IsSmooth();

protected:
    void ObjectRender();

    int m_value;
    int m_valueTarget;
    int m_smoothSpeed;
    bool m_smooth;
};

}
}

#endif
