/* 
 * File:   ParticlesParallelScene.h
 * Author: b4n92uid
 *
 * Created on 30 avril 2010, 23:09
 */

#ifndef _PARTICLESPARALLELSCENE_H
#define	_PARTICLESPARALLELSCENE_H

#include "ParallelScene.h"

namespace tbe
{
namespace scene
{

class ParticlesEmiter;

/**
 * \brief Scene parallele de rendue des particules
 */
class ParticlesParallelScene : public ParallelSceneRegister<ParticlesEmiter>
{
public:
    ParticlesParallelScene();
    ~ParticlesParallelScene();

    void Render();

    void HardwareParticleMaxSize();

    void SetParticleMinSize(float particleMinSize);
    float GetParticleMinSize() const;

    void SetParticleMaxSize(float particleMaxSize);
    float GetParticleMaxSize() const;

private:
    float m_particleMinSize;
    float m_particleMaxSize;
};

}
}

#endif	/* _PARTICLESPARALLELSCENE_H */
