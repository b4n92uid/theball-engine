/* 
 * File:   BurningEmitter.h
 * Author: b4n92uid
 *
 * Created on 28 mars 2011, 18:51
 */

#ifndef BURNINGEMITTER_H
#define	BURNINGEMITTER_H


#include <vector>

#include "Mathematics.h"
#include "Texture.h"

#include "Node.h"
#include "Material.h"
#include "ParticlesParallelScene.h"
#include "Particles.h"

namespace tbe
{
namespace scene
{

class ParticlesParallelScene;

class BurningEmitter : public ParticlesEmiter
{
public:
    BurningEmitter(ParticlesParallelScene* scene);
    BurningEmitter(const BurningEmitter& copy);
    virtual ~BurningEmitter();

    BurningEmitter & operator=(const BurningEmitter& copy);
};

}
}

#endif	/* BURNINGEMITTER_H */

