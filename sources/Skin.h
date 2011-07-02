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

    GuiSkin()
    {
        buttonMetaCount = 3;
        gaugeMetaCount = 2;
        editBoxMetaCount = 3;
        switchBoxMetaCount = 3;
        vectorBoxMetaCount = 3;
    }

    Texture button;
    Vector2f buttonSize;
    int buttonMetaCount;

    Texture gauge;
    Vector2f gaugeSize;
    int gaugeMetaCount;

    Texture editBox;
    Vector2f editBoxSize;
    int editBoxMetaCount;

    Texture switchBox;
    Vector2f switchBoxSize;
    int switchBoxMetaCount;

    Texture vectorBox;
    Vector2f vectorBoxSize;
    int vectorBoxMetaCount;

    Vector2f stateShowSize;

    Pencil pencil;
};

}
}

#endif // SKIN_H
