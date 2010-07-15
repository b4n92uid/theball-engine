/* 
 * File:   MotionBlurEffect.h
 * Author: b4n92uid
 *
 * Created on 5 février 2010, 12:57
 */

#ifndef _MOTIONBLUREFFECT_H
#define	_MOTIONBLUREFFECT_H

#include "PostProcessManager.h"
#include "BlurEffect.h"

namespace tbe
{

namespace ppe
{

class MotionBlurEffect : public Effect
{
public:
    MotionBlurEffect();
    virtual ~MotionBlurEffect();

    void Process(Rtt* rtt);
    
    void SetIntensity(float intensity);
    float GetIntensity() const;
    
private:
    float m_intensity;
};

}
}

#endif	/* _MOTIONBLUREFFECT_H */

