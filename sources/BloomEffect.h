/* 
 * File:   BloomEffect.h
 * Author: b4n92uid
 *
 * Created on 8 janvier 2010, 21:31
 */

#ifndef _BLOOMEFFECT_H
#define	_BLOOMEFFECT_H

#include "Shader.h"

#include "PostProcessManager.h"

namespace tbe
{

namespace ppe
{

class BloomEffect : public Effect
{
public:
    BloomEffect();
    virtual ~BloomEffect();

    void process(Rtt* rtt);

    void setIntensity(float intensity);
    float getIntensity() const;

    void setThreshold(float threshold);
    float getThreshold() const;
    
    void setBlurPass(unsigned blurPass);
    unsigned getBlurPass() const;

private:
    Shader m_blurShader;
    float m_threshold;
    float m_intensity;
    unsigned m_blurPass;
};

}
}

#endif	/* _BLOOMEFFECT_H */

