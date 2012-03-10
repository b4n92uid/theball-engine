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
#include "Clock.h"

namespace tbe
{

namespace ppe
{

class MotionBlurEffect : public Effect
{
public:
    MotionBlurEffect();
    virtual ~MotionBlurEffect();

    void process(Rtt* rtt);
    
    void setIntensity(float intensity);
    float getIntensity() const;
    
private:
    float m_intensity;
    ticks::Clock m_clock;
};

}
}

#endif	/* _MOTIONBLUREFFECT_H */

