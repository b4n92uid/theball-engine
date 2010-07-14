/* 
 * File:   BloomEffect.h
 * Author: b4n92uid
 *
 * Created on 8 janvier 2010, 21:31
 */

#ifndef _BLOOMEFFECT_H
#define	_BLOOMEFFECT_H

#include "Shader.h"

#include "BlurEffect.h"
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

    void Process(Rtt* rtt);

    void SetIntensity(float intensity);
    float GetIntensity() const;

    void SetThreshold(float threshold);
    float GetThreshold() const;

    void SetBlurPass(unsigned pass);
    unsigned GetBlurPass() const;

    void SetBlurOffset(float offset);
    float GetBlurOffset() const;
    
    void SetUseThreshold(bool useThreshold);
    bool IsUseThreshold() const;

private:
    BlurEffect m_blur;
    float m_threshold;
    float m_intensity;
    bool m_useThreshold;
};

}
}

#endif	/* _BLOOMEFFECT_H */

