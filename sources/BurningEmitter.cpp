/* 
 * File:   BurningEmitter.cpp
 * Author: b4n92uid
 * 
 * Created on 28 mars 2011, 18:50
 */

#include "BurningEmitter.h"

#include "Tools.h"

#include "SceneManager.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

BurningEmitter::BurningEmitter(ParticlesParallelScene* scene) : ParticlesEmiter(scene)
{
}

BurningEmitter::BurningEmitter(const BurningEmitter& copy) : ParticlesEmiter(copy)
{
    *this = copy;
}

BurningEmitter::~BurningEmitter()
{
}

BurningEmitter& BurningEmitter::operator=(const BurningEmitter& copy)
{
    ParticlesEmiter::operator =(copy);

    return *this;
}
