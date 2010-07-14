#ifndef SKIN_H
#define SKIN_H

#include "Texture.h"
#include "Pencil.h"

namespace tbe
{
namespace gui
{

/**
 * \brief Structure contenant l'apparence et la configuration
 * des controles de l'interface graphique
 */

struct GuiSkin
{
    Texture button;
    Vector2f buttonSize;

    Texture gauge;
    Vector2f gaugeSize;

    Texture editBox;
    Vector2f editBoxSize;

    Texture switchBox;
    Vector2f switchBoxSize;

    Texture vectorBox;
    Vector2f vectorBoxSize;

    Vector2f stateShowSize;

    Pencil pencile;
};

}
}

#endif // SKIN_H
