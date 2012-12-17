/*
 * File:   BulletParallelScene.cpp
 * Author: b4n92uid
 *
 * Created on 27 novembre 2009, 19:37
 */

#include "BulletParallelScene.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

BulletParallelScene::BulletParallelScene()
{
    m_broadphase = new btDbvtBroadphase();
    m_sequentialImpulseConstraintSolver = new btSequentialImpulseConstraintSolver;
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

    m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_sequentialImpulseConstraintSolver, m_collisionConfiguration);

    setWorldTimestep(1.0 / 60.0);
    setGravity(Vector3f(0, -9.81, 0));
}

BulletParallelScene::~BulletParallelScene()
{
    // delete m_world;
    delete m_sequentialImpulseConstraintSolver;
    delete m_broadphase;
    delete m_dispatcher;
    delete m_collisionConfiguration;
}

void BulletParallelScene::setGravity(Vector3f gravity)
{
    this->m_gravity = gravity;
    m_world->setGravity(tbe2btVec(gravity));
}

Vector3f BulletParallelScene::getGravity() const
{
    return m_gravity;
}

void BulletParallelScene::render()
{
    if(!m_enable)
        return;

    m_world->stepSimulation(m_worldTimestep);
}

void BulletParallelScene::setWorldTimestep(float worldTimestep)
{
    this->m_worldTimestep = worldTimestep;
}

float BulletParallelScene::getWorldTimestep() const
{
    return m_worldTimestep;
}

btDiscreteDynamicsWorld* BulletParallelScene::getWorld() const
{
    return m_world;
}
