/* 
 * File:   VectorBox.h
 * Author: b4n92uid
 *
 * Created on 18 juin 2010, 15:46
 */

#ifndef VECTORBOX_H
#define	VECTORBOX_H

#include "Control.h"
#include "Switch.h"
#include "Layout.h"

namespace tbe
{
namespace gui
{

/**
 * \brief Controle de manipulation de Vector3f par l'utlisateur
 * via l'interface graphique
 */
class VectorBox : public Control
{
public:
    VectorBox();
    virtual ~VectorBox();

    bool OnEvent(const EventManager& event);
    void SetSkin(const GuiSkin& gui);

    void SetValue(Vector3f value);
    Vector3f GetValue();

    void SetRange(float min, float max);

protected:
    void ObjectRender();

protected:
    SwitchNumeric<float> m_xValue;
    SwitchNumeric<float> m_yValue;
    SwitchNumeric<float> m_zValue;
    Layout m_lay;
};

}
}

#endif	/* VECTORBOX_H */

