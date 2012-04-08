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

    void process(Rtt* rtt);

    void setFusionMode(FusionMode fusionMode);
    FusionMode getFusionMode() const;

    void setColor(Vector4f color);
    Vector4f getColor() const;

    void setAlpha(float alpha);
    float getAlpha() const;

    void setUseShader(bool useShader);
    bool isUseShader() const;

private:
    Vector4f m_color;
    Texture m_fallback;
    FusionMode m_fusionMode;
    bool m_useShader;
};

}
}

#endif	/* _COLOREFFECT_H */

