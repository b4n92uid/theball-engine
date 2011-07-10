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

    void setSkin(const GuiSkin& skin);
    bool onEvent(const EventManager& event);

protected:
    void objectRender();

protected:

    enum State
    {
        NO_EVENT, OVER, PUSH
    } m_state;
};

}
}

#endif
