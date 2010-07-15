#ifndef EDITBOX_H
#define EDITBOX_H

#include "Texture.h"

#include "Control.h"
#include "Pencil.h"

namespace tbe
{
namespace gui
{

/// \brief Champ editibale par l'utilisateur

class EditBox : public Control
{
public:
    EditBox();
    EditBox(std::string label, Pencil font, Texture background);
    ~EditBox();

    void SetSkin(const GuiSkin& skin);
    bool OnEvent(const EventManager& event);

protected:
    void ObjectRender();

    int m_shiftDown;
    int m_key;

    enum State
    {
        NO_EVENT, OVER, EDIT
    } m_state;
};

}
}

#endif // EDITBOX_H
