/*
 * File:   NewtonParallelScene.cpp
 * Author: b4n92uid
 *
 * Created on 27 novembre 2009, 19:37
 */

#include "NewtonParallelScene.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

//// NewtonParallelScene ---------------------------------------------------

NewtonParallelScene::NewtonParallelScene()
{
    m_newtonWorld = NewtonCreate();

    NewtonWorldSetUserData(m_newtonWorld, this);

    SetWorldSize(AABB(-100, 100));

    m_worldTimestep = 1.0 / 60.0;
    m_gravity = 1;
}

NewtonParallelScene::~NewtonParallelScene()
{
    NewtonDestroy(m_newtonWorld);
}

void NewtonParallelScene::SetGravity(float gravity)
{
    this->m_gravity = gravity;
}

float NewtonParallelScene::GetGravity() const
{
    return m_gravity;
}

void NewtonParallelScene::SetWorldSize(AABB wordlSize)
{
    m_worldSize = wordlSize;
    NewtonSetWorldSize(m_newtonWorld, m_worldSize.min, m_worldSize.max);
}

AABB NewtonParallelScene::GetWorldSize() const
{
    return m_worldSize;
}

void NewtonParallelScene::Render()
{
    if(!m_enable)
        return;

    NewtonUpdate(m_newtonWorld, m_worldTimestep);
}

void NewtonParallelScene::SetWorldTimestep(float worldTimestep)
{
    this->m_worldTimestep = worldTimestep;
}

float NewtonParallelScene::GetWorldTimestep() const
{
    return m_worldTimestep;
}

NewtonWorld* NewtonParallelScene::GetNewtonWorld() const
{
    return m_newtonWorld;
}

static dFloat RayFilterAnyBody(const NewtonBody* body, const dFloat* hitNormal, int collisionID, void* userData, dFloat intersectParam)
{
    *(float*)(userData) = intersectParam;
    return intersectParam;
}

static dFloat RayFilterZeroMassBody(const NewtonBody* body, const dFloat* hitNormal, int collisionID, void* userData, dFloat intersectParam)
{
    float inertia[3], masse;
    NewtonBodyGetMassMatrix(body, &masse, &inertia[0], &inertia[1], &inertia[2]);

    float& userDataIntersectParam = *static_cast<float*>(userData);

    if(!(masse > 0.0f) && intersectParam < userDataIntersectParam)
    {
        userDataIntersectParam = intersectParam;
        return 0.0;
    }

    return intersectParam;
}

Vector3f NewtonParallelScene::FindZeroMassBody(Vector3f start, Vector3f end)
{
    float intersectParam = 1.2f;
    NewtonWorldRayCast(m_newtonWorld, start, end, RayFilterZeroMassBody, &intersectParam, 0);
    return start + (start - end) * intersectParam;
}

Vector3f NewtonParallelScene::FindAnyBody(Vector3f start, Vector3f end)
{
    float intersectParam = 1.2f;
    NewtonWorldRayCast(m_newtonWorld, start, end, RayFilterAnyBody, &intersectParam, 0);
    return start + (start - end) * intersectParam;
}

Vector3f NewtonParallelScene::FindFloor(Vector3f pos)
{
    Vector3f p0 = pos, p1 = pos;

    p0.y = m_worldSize.max.y;
    p1.y = m_worldSize.min.y;

    float intersect = 1.2f;

    NewtonWorldRayCast(m_newtonWorld, &p0.x, &p1.x, RayFilterZeroMassBody, &intersect, 0);

    return Vector3f(pos.x, p0.y + (p1.y - p0.y) * intersect + pos.y, pos.z);
}
