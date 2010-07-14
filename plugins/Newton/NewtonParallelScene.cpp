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

    NewtonSetSolverModel(m_newtonWorld, 1);
    NewtonSetFrictionModel(m_newtonWorld, 1);
    NewtonWorldSetUserData(m_newtonWorld, this);

    SetWorldSize(AABB(-100, 100));

    m_worldTimestep = 1.0 / 60.0;
    m_sharedNode = false;
    m_gravity = 1;
}

NewtonParallelScene::~NewtonParallelScene()
{
    if(!m_sharedNode)
        Clear();

    NewtonDestroy(m_newtonWorld);
}

void NewtonParallelScene::Clear()
{
    for(std::map<std::string, NewtonNode*>::iterator i = m_newtonNodes.begin(); i != m_newtonNodes.end(); i++)
        delete i->second;

    m_newtonNodes.clear();

    NewtonDestroyAllBodies(m_newtonWorld);
}

void NewtonParallelScene::SetGravity(float gravity)
{
    this->m_gravity = gravity;
}

float NewtonParallelScene::GetGravity() const
{
    return m_gravity;
}

void NewtonParallelScene::SetSharedNode(bool sharedNode)
{
    this->m_sharedNode = sharedNode;
}

bool NewtonParallelScene::IsSharedNode() const
{
    return m_sharedNode;
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

    std::map<std::string, NewtonNode*>::iterator endOfNewtonEnity = m_newtonNodes.end();
    for(std::map<std::string, NewtonNode*>::iterator i = m_newtonNodes.begin(); i != endOfNewtonEnity; i++)
        i->second->UpdateMatrix();
}

void NewtonParallelScene::AddNode(std::string name, NewtonNode* node)
{
    if(name.empty())
        name = tools::NameGen(m_newtonNodes);

    else if(m_newtonNodes.find(name) != m_newtonNodes.end())
        throw Exception("SceneManager::AddNewtonNode\nName already exist (%s)", name.c_str());

    if(!node)
        throw Exception("SceneManager::AddNewtonNode\nTry to add a NULL ptr child");

    if(!node->GetBody())
        throw Exception("SceneManager::AddNewtonNode\nTry to add a NULL ptr body child");

    node->SetNewtonScene(this);

    m_newtonNodes[name] = node;
}

NewtonNode* NewtonParallelScene::GetNode(std::string name)
{
    if(m_newtonNodes.find(name) != m_newtonNodes.end())
        return m_newtonNodes[name];

    else
        throw tbe::Exception("NewtonSubScene::GetNewtonMesh\nNewtonMesh not found (%s)\n\n", name.c_str());
}

NewtonNode* NewtonParallelScene::ReleaseNode(std::string name)
{
    if(m_newtonNodes.find(name) == m_newtonNodes.end())
        throw Exception("MainParallelScene::RealeaseNode\nNode not found (%s)\n\n", name.c_str());

    NewtonNode* releaseNode = m_newtonNodes[name];

    m_newtonNodes.erase(name);

    return releaseNode;
}

void NewtonParallelScene::DeleteNode(std::string name)
{
    if(m_newtonNodes.find(name) == m_newtonNodes.end())
        throw Exception("MainParallelScene::DeleteNode\nNode not found (%s)\n\n", name.c_str());

    delete m_newtonNodes[name];
    m_newtonNodes.erase(name);
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
    return start - (start - end) * intersectParam;
}

Vector3f NewtonParallelScene::FindAnyBody(Vector3f start, Vector3f end)
{
    float intersectParam = 1.2f;
    NewtonWorldRayCast(m_newtonWorld, start, end, RayFilterAnyBody, &intersectParam, 0);
    return start - (start - end) * intersectParam;
}

Vector3f NewtonParallelScene::FindFloor(Vector3f pos)
{
    Vector3f p0 = pos, p1 = pos;

    p0.y = m_worldSize.max.y;
    p1.y = m_worldSize.min.y;

    float intersect = 1.2f;

    NewtonWorldRayCast(m_newtonWorld, &p0.x, &p1.x, RayFilterZeroMassBody, &intersect, 0);

    //return Vector3f(pos.x, 1000.0f - 2000.0f * intersect + pos.y, pos.z);
    return Vector3f(pos.x, p0.y + (p1.y - p0.y) * intersect + pos.y, pos.z);
}

void NewtonParallelScene::ApplyForceAndTorque(const NewtonBody* body, float, int)
{
    NewtonNode* core = (NewtonNode*)NewtonBodyGetUserData(body);

    if(!core)
        throw tbe::Exception("NewtonParallelScene::ApplyForceAndTorque; core == NULL");

    Vector3f applyForce = core->GetApplyForce();
    Vector3f applyTorque = core->GetApplyTorque();

    if(core->IsApplyGravity())
        applyForce.y -= core->GetMasse() * 9.81 * core->GetNewtonScene()->GetGravity();

    NewtonBodyAddForce(body, applyForce);
    NewtonBodyAddTorque(body, applyTorque);

    core->SetApplyForce(0);
}
