/* 
 * File:   ParticlesParallelScene.h
 * Author: b4n92uid
 *
 * Created on 30 avril 2010, 23:09
 */

#ifndef _PARTICLESPARALLELSCENE_H
#define	_PARTICLESPARALLELSCENE_H

#include "ParallelScene.h"
#include "Particles.h"

namespace tbe
{
namespace scene
{

/**
 * \brief Scene parallele de rendue des particules
 */
class ParticlesParallelScene : public ParallelScene
{
public:
    ParticlesParallelScene();
    ~ParticlesParallelScene();

    void Render();
};

}
}

#endif	/* _PARTICLESPARALLELSCENE_H */
