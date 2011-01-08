/* 
 * File:   ColorEffect.h
 * Author: b4n92uid
 *
 * Created on 22 janvier 2010, 18:54
 */

#ifndef _COLOREFFECT_H
#define	_COLOREFFECT_H

#include "Shader.h"

#include "PostProcessManager.h"

namespace tbe
{

namespace ppe
{

class ColorEffect : public Effect
{
public:
    ColorEffect();
    virtual ~ColorEffect();

    enum FusionMode
    {
        ADDITIVE_COLOR,
        SUBTRACTION_COLOR,
        MULTIPLICATION_COLOR,
        DIVISION_COLOR,
        AVERAGE_COLOR,
        BLACK_WHITE,
        INVERT,
    };

    void Process(Rtt* rtt);

    void SetFusionMode(FusionMode fusionMode);
    FusionMode GetFusionMode() const;

    void SetColor(Vector4f color);
    Vector4f GetColor() const;

    void SetInternalPass(bool internalPass);
    bool IsInternalPass() const;

private:
    Vector4f m_color;
    FusionMode m_fusionMode;
    bool m_internalPass;
};

}
}

#endif	/* _COLOREFFECT_H */

