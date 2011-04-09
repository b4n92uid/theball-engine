/* 
 * File:   DofEffect.h
 * Author: b4n92uid
 *
 * Created on 10 janvier 2010, 20:23
 */

#ifndef _DOFEFFECT_H
#define	_DOFEFFECT_H

#include "BlurEffect.h"
#include "PostProcessManager.h"

namespace tbe
{
namespace ppe
{

class DofEffect : public Effect
{
public:
    DofEffect();
    virtual ~DofEffect();

    void process(Rtt* rtt);

private:
    BlurEffect m_blur;
};

}
}

#endif	/* _DOFEFFECT_H */
