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

    setWorldSize(AABB(-100, 100));

    m_worldTimestep = 1.0 / 60.0;
    m_gravity = 1;
}

NewtonParallelScene::~NewtonParallelScene()
{
    NewtonDestroy(m_newtonWorld);
}

void NewtonParallelScene::setGravity(float gravity)
{
    this->m_gravity = gravity;
}

float NewtonParallelScene::getGravity() const
{
    return m_gravity;
}

void NewtonParallelScene::setWorldSize(AABB wordlSize)
{
    m_worldSize = wordlSize;
    // FIXME
    // NewtonSetWorldSize(m_newtonWorld, m_worldSize.min, m_worldSize.max);
}

AABB NewtonParallelScene::getWorldSize() const
{
    return m_worldSize;
}

void NewtonParallelScene::render()
{
    if(!m_enable)
        return;

    NewtonUpdate(m_newtonWorld, m_worldTimestep);
}

void NewtonParallelScene::setWorldTimestep(float worldTimestep)
{
    this->m_worldTimestep = worldTimestep;
}

float NewtonParallelScene::getWorldTimestep() const
{
    return m_worldTimestep;
}

NewtonWorld* NewtonParallelScene::getNewtonWorld() const
{
    return m_newtonWorld;
}

static dFloat RayFilterAllBody(const NewtonBody* const body, const NewtonCollision* const shapeHit, const dFloat* const hitContact, const dFloat* const hitNormal, dLong collisionID, void* const userData, dFloat intersectParam)
{
    vector<float>& intersectArray = *static_cast<vector<float>*> (userData);
    intersectArray.push_back(intersectParam);
    return 1;
}

Vector3f::Array NewtonParallelScene::rayCast(Vector3f start, Vector3f end)
{
    vector<float> intersectArray;
    NewtonWorldRayCast(m_newtonWorld, start, end, RayFilterAllBody, &intersectArray, NULL, 0);

    std::sort(intersectArray.begin(), intersectArray.end());

    Vector3f::Array vecarray;
    vecarray.resize(intersectArray.size());

    for(unsigned i = 0; i < vecarray.size(); i++)
        vecarray[i] = start + (end - start) * intersectArray[i];

    return vecarray;
}

static dFloat RayFilterAnyBody(const NewtonBody* const body, const NewtonCollision* const shapeHit, const dFloat* const hitContact, const dFloat* const hitNormal, dLong collisionID, void* const userData, dFloat intersectParam)
{
    *(float*) (userData) = intersectParam;
    return 1;
}

Vector3f NewtonParallelScene::findAnyBody(Vector3f start, Vector3f end)
{
    float intersectParam = 1.0f;
    NewtonWorldRayCast(m_newtonWorld, start, end, RayFilterAnyBody, &intersectParam, NULL, 0);
    var_dump(intersectParam);
    return start + (end - start) * intersectParam;
}

static dFloat RayFilterZeroMassBody(const NewtonBody* const body, const NewtonCollision* const shapeHit, const dFloat* const hitContact, const dFloat* const hitNormal, dLong collisionID, void* const userData, dFloat intersectParam)
{
    float inertia[3], masse;
    NewtonBodyGetMassMatrix(body, &masse, &inertia[0], &inertia[1], &inertia[2]);

    float& userDataIntersectParam = *static_cast<float*> (userData);

    if(math::isZero(masse) && intersectParam < userDataIntersectParam)
        userDataIntersectParam = intersectParam;

    return intersectParam;
}

Vector3f NewtonParallelScene::findZeroMassBody(Vector3f start, Vector3f end)
{
    float intersectParam = 1.0f;
    NewtonWorldRayCast(m_newtonWorld, start, end, RayFilterZeroMassBody, &intersectParam, NULL, 0);
    return start + (end - start) * intersectParam;
}

Vector3f NewtonParallelScene::findFloor(Vector3f pos)
{
    Vector3f p0 = pos, p1 = pos;

    p0.y = m_worldSize.max.y;
    p1.y = m_worldSize.min.y;

    float intersect = 1.0f;

    NewtonWorldRayCast(m_newtonWorld, &p0.x, &p1.x, RayFilterZeroMassBody, &intersect, NULL, 0);

    return Vector3f(pos.x, p0.y + (p1.y - p0.y) * intersect + pos.y, pos.z);
}
