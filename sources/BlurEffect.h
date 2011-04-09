/* 
 * File:   BlurEffect.h
 * Author: b4n92uid
 *
 * Created on 4 janvier 2010, 15:06
 */

#ifndef _BLUREFFECT_H
#define	_BLUREFFECT_H

#include "Shader.h"

#include "PostProcessManager.h"

namespace tbe
{

namespace ppe
{

class BlurEffect : public Effect
{
public:
    BlurEffect();
    virtual ~BlurEffect();

    void process(Rtt* rtt);

    void setPasse(unsigned passe);
    unsigned getPasse() const;

private:
    unsigned m_passe;
};

}
}

#endif	/* _BLUREFFECT_H */

