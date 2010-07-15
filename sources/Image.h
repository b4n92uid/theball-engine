#ifndef _IMAGE_H
#define _IMAGE_H

#include "Texture.h"
#include "Control.h"
#include "Skin.h"

namespace tbe
{
namespace gui
{

/// \brief Controle pout l'affichage d'image

class Image : public Control
{
public:
    Image();
    Image(Texture background);

    bool OnEvent(const EventManager& event);

    void SetSkin(const GuiSkin& skin);

    enum DrawDiriction
    {
        DRAW_UPPER_LEFT,
        DRAW_UPPER_RIGHT,
    };

    /// Spécifier la diriction du rendue
    void SetDrawDiriction(DrawDiriction drawDiriction);
    DrawDiriction GetDrawDiriction() const;

protected:
    void ObjectRender();

    DrawDiriction m_drawDiriction;
};

}
}

#endif // _IMAGE_H
