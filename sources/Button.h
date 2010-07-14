#ifndef BUTTON_H
#define BUTTON_H

#include "Texture.h"

#include "Control.h"
#include "Pencil.h"

namespace tbe
{
namespace gui
{

/// \brief Boutton poussoire

class Button : public Control
{
public:
    Button();
    Button(std::string label, Pencil font, Texture background);
    ~Button();

    void SetSkin(const GuiSkin& skin);
    bool OnEvent(const EventManager& event);

protected:
    void ObjectRender();

    enum State
    {
        NO_EVENT,
        OVER,
        PUSH
    } m_state;
};

}
}

#endif
