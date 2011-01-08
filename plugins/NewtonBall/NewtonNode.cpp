/* 
 * File:   NewtonNode.cpp
 * Author: b4n92uid
 * 
 * Created on 27 novembre 2009, 19:35
 */

#include <Tbe.h>
#include "NewtonNode.h"
#include "NewtonParallelScene.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

//// NewtonNode ----------------------------------------------------------------

NewtonNode::NewtonNode(NewtonParallelScene* newtonScene)
{
    m_parallelScene = newtonScene;
    m_newtonWorld = newtonScene->GetNewtonWorld();
    m_body = NULL;

    m_updatedMatrix = NULL;

    m_masse = 0;

    m_freeze = false;
    m_applyGravity = true;

    m_parallelScene->Register(this);
}

NewtonNode::NewtonNode(NewtonParallelScene* newtonScene, Node* node)
{
    m_parallelScene = newtonScene;
    m_newtonWorld = newtonScene->GetNewtonWorld();
    m_body = NULL;

    m_updatedMatrix = &node->GetMatrix();

    m_masse = 0;

    m_freeze = false;
    m_applyGravity = true;

    m_parallelScene->Register(this);
}

NewtonNode::NewtonNode(NewtonParallelScene* newtonScene, Matrix4f* matrix)
{
    m_parallelScene = newtonScene;
    m_newtonWorld = newtonScene->GetNewtonWorld();
    m_body = NULL;

    m_updatedMatrix = matrix;

    m_masse = 0;

    m_freeze = false;
    m_applyGravity = true;

    m_parallelScene->Register(this);
}

NewtonNode::NewtonNode(const NewtonNode& copy)
{
    *this = copy;

    m_parallelScene->Register(this);
}

NewtonNode::~NewtonNode()
{
    DestroyBody();

    m_parallelScene->UnRegister(this);
}

bool NewtonNode::operator=(const NewtonNode& copy)
{
    m_applyForce = copy.m_applyForce;
    m_applyTorque = copy.m_applyTorque;
    m_parallelScene = copy.m_parallelScene;
    m_newtonWorld = copy.m_newtonWorld;

    m_masse = copy.m_masse;
    m_freeze = copy.m_freeze;
    m_applyGravity = copy.m_applyGravity;

    return true;
}

Node* NewtonNode::Clone()
{
    return new NewtonNode(*this);
}

void NewtonNode::BuildBoxNode(Vector3f size, float masse)
{
    if(!m_newtonWorld)
        throw Exception("NewtonNode::BuildBoxNode; newtonWorld = NULL");

    m_masse = masse;

    size *= 2;

    // Corp de collision
    NewtonCollision* collision = NewtonCreateBox(m_newtonWorld, size.x, size.y, size.z, 0, NULL);
    m_body = NewtonCreateBody(m_newtonWorld, collision, *m_updatedMatrix);

    // Masse & Inertia & Inertia
    Vector3f origine;
    Vector3f inertia;

    NewtonConvexCollisionCalculateInertialMatrix(collision, inertia, origine);
    inertia *= m_masse;

    NewtonBodySetMassMatrix(m_body, m_masse, inertia.x, inertia.y, inertia.z);

    // Donneés utilisateur
    NewtonBodySetUserData(m_body, this);

    // Callback
    NewtonBodySetForceAndTorqueCallback(m_body, NewtonNode::ApplyForceAndTorque);

    size /= 2;

    // Matrice
    NewtonBodySetMatrix(m_body, *m_updatedMatrix);

    NewtonReleaseCollision(m_newtonWorld, collision);
}

void NewtonNode::BuildSphereNode(Vector3f size, float masse)
{
    if(!m_newtonWorld)
        throw Exception("NewtonNode::BuildBoxNode; newtonWorld = NULL");

    m_masse = masse;

    // Corp de collision
    NewtonCollision * collision = NewtonCreateSphere(m_newtonWorld, size.x, size.y, size.z, 0, NULL);
    m_body = NewtonCreateBody(m_newtonWorld, collision, *m_updatedMatrix);

    // Masse & Inertia
    Vector3f origine;
    Vector3f inertia;

    NewtonConvexCollisionCalculateInertialMatrix(collision, inertia, origine);
    inertia *= m_masse;

    NewtonBodySetMassMatrix(m_body, m_masse, inertia.x, inertia.y, inertia.z);

    // Donneés utilisateur
    NewtonBodySetUserData(m_body, this);

    // Callback
    NewtonBodySetForceAndTorqueCallback(m_body, NewtonNode::ApplyForceAndTorque);

    // Matrice
    NewtonBodySetMatrix(m_body, *m_updatedMatrix);

    NewtonReleaseCollision(m_newtonWorld, collision);
}

void NewtonNode::BuildCylinderNode(Vector3f size, float masse)
{
    if(!m_newtonWorld)
        throw Exception("NewtonNode::BuildBoxNode; newtonWorld = NULL");

    m_masse = masse;

    // Corp de collision
    NewtonCollision * collision = NewtonCreateCylinder(m_newtonWorld, size.y, size.z, 0, NULL);
    m_body = NewtonCreateBody(m_newtonWorld, collision, *m_updatedMatrix);

    // Masse & Inertia
    Vector3f origine;
    Vector3f inertia;

    NewtonConvexCollisionCalculateInertialMatrix(collision, inertia, origine);
    inertia *= m_masse;

    NewtonBodySetMassMatrix(m_body, m_masse, inertia.x, inertia.y, inertia.z);

    // Donneés utilisateur
    NewtonBodySetUserData(m_body, this);

    // Callback
    NewtonBodySetForceAndTorqueCallback(m_body, NewtonNode::ApplyForceAndTorque);

    // Matrice
    NewtonBodySetMatrix(m_body, *m_updatedMatrix);

    NewtonReleaseCollision(m_newtonWorld, collision);
}

inline Vector3f::Array ExtractPos(const Vertex::Array& vertexes)
{
    Vector3f::Array onlyPos;

    for(unsigned i = 0; i < vertexes.size(); i++)
        onlyPos.push_back(vertexes[i].pos);

    return onlyPos;
}

void NewtonNode::BuildConvexNode(const Vertex::Array& vertexes, float masse)
{
    if(!m_newtonWorld)
        throw Exception("NewtonNode::BuildBoxNode; newtonWorld = NULL");

    m_masse = masse;

    Vector3f::Array onlyPos = ExtractPos(vertexes);

    // Corp de collision
    NewtonCollision* collision = NewtonCreateConvexHull(m_newtonWorld, vertexes.size(), &onlyPos[0].x, sizeof (Vector3f), 0, 0, NULL);
    m_body = NewtonCreateBody(m_newtonWorld, collision, *m_updatedMatrix);

    // Masse & Inertia
    Vector3f origine;
    Vector3f inertia;

    NewtonConvexCollisionCalculateInertialMatrix(collision, inertia, origine);
    inertia *= m_masse;

    NewtonBodySetMassMatrix(m_body, m_masse, inertia.x, inertia.y, inertia.z);

    // Donne utilisateur
    NewtonBodySetUserData(m_body, this);

    // Callback
    NewtonBodySetForceAndTorqueCallback(m_body, NewtonNode::ApplyForceAndTorque);

    // Matrice
    NewtonBodySetMatrix(m_body, *m_updatedMatrix);

    NewtonReleaseCollision(m_newtonWorld, collision);
}

void NewtonNode::BuildTreeNode(const Face::Array& faces)
{
    if(!m_newtonWorld)
        throw Exception("NewtonNode::BuildBoxNode; newtonWorld = NULL");

    m_masse = 0;

    // Corp de collision

    NewtonCollision * nCollision = NewtonCreateTreeCollision(m_newtonWorld, 0);
    NewtonTreeCollisionBeginBuild(nCollision);

    for(unsigned int i = 0; i < faces.size(); i++)
    {
        vector<Vector3f> vertexesPos;

        for(unsigned j = 0; j < faces[j].size(); j++)
            vertexesPos.push_back(faces[i][j].pos);

        NewtonTreeCollisionAddFace(nCollision, vertexesPos.size(), &vertexesPos[0].x, sizeof (Vector3f), 0);
    }

    // 1 = optimisation
    NewtonTreeCollisionEndBuild(nCollision, 0);
    m_body = NewtonCreateBody(m_newtonWorld, nCollision, *m_updatedMatrix);
    NewtonReleaseCollision(m_newtonWorld, nCollision);

    // Donne utilisateur
    NewtonBodySetUserData(m_body, this);

    // Matrice
    NewtonBodySetMatrix(m_body, *m_updatedMatrix);
}

void NewtonNode::DestroyBody()
{
    if(m_body)
    {
        NewtonDestroyBody(m_newtonWorld, m_body);

        m_body = NULL;
        m_masse = 0;
    }
}

bool NewtonNode::IsCollidWith(const NewtonNode* target)
{
    float contact[3], normal[3], penetration, matrix1[16], matrix2[16];

    NewtonBodyGetMatrix(m_body, matrix1);
    NewtonBodyGetMatrix(target->m_body, matrix2);

    return NewtonCollisionCollide(
                                  NewtonBodyGetWorld(m_body),
                                  1,
                                  NewtonBodyGetCollision(m_body),
                                  matrix1,
                                  NewtonBodyGetCollision(target->m_body),
                                  matrix2,
                                  contact,
                                  normal,
                                  &penetration,
                                  0
                                  );
}

void NewtonNode::Render()
{
}

void NewtonNode::Process()
{
    if(!m_enable)
        return;

    for(unsigned i = 0; i < m_childs.size(); i++)
        m_childs[i]->Process();

    if(m_body)
        NewtonBodyGetMatrix(m_body, *m_updatedMatrix);
}

void NewtonNode::SetMatrix(Matrix4f matrix)
{
    *m_updatedMatrix = matrix;

    if(m_body)
        NewtonBodySetMatrix(m_body, *m_updatedMatrix);
}

Matrix4f NewtonNode::GetMatrix()
{
    return *m_updatedMatrix;
}

void NewtonNode::SetPos(Vector3f pos)
{
    m_updatedMatrix->SetPos(pos);

    if(m_body)
        NewtonBodySetMatrix(m_body, *m_updatedMatrix);
}

Vector3f NewtonNode::GetPos()
{
    return m_updatedMatrix->GetPos();
}

void NewtonNode::SetVelocity(Vector3f vel)
{
    NewtonBodySetVelocity(m_body, vel);
}

Vector3f NewtonNode::GetVelocity()
{
    Vector3f vel;
    NewtonBodyGetVelocity(m_body, vel);
    return vel;
}

NewtonBody* NewtonNode::GetBody()
{
    return m_body;
}

float NewtonNode::GetMasse()
{
    return m_masse;
}

void NewtonNode::SetUpdatedMatrix(Matrix4f* updatedMatrix)
{
    this->m_updatedMatrix = updatedMatrix;
}

Matrix4f* NewtonNode::GetUpdatedMatrix() const
{
    return m_updatedMatrix;
}

void NewtonNode::SetApplyTorque(Vector3f applyTorque)
{
    this->m_applyTorque = applyTorque;
}

Vector3f NewtonNode::GetApplyTorque() const
{
    return m_applyTorque;
}

void NewtonNode::SetApplyForce(Vector3f applyForce)
{
    this->m_applyForce = applyForce;
}

Vector3f NewtonNode::GetApplyForce() const
{
    return m_applyForce;
}

void NewtonNode::SetApplyGravity(bool applyGravity)
{
    this->m_applyGravity = applyGravity;
}

bool NewtonNode::IsApplyGravity() const
{
    return m_applyGravity;
}

void NewtonNode::SetFreeze(bool freeze)
{
    this->m_freeze = freeze;

    NewtonBodySetForceAndTorqueCallback(m_body, freeze ? NULL : NewtonNode::ApplyForceAndTorque);
    NewtonBodySetVelocity(m_body, Vector3f(0));
}

bool NewtonNode::IsFreeze() const
{
    return m_freeze;
}

NewtonParallelScene* NewtonNode::GetParallelScene() const
{
    return m_parallelScene;
}

void NewtonNode::ApplyForceAndTorque(const NewtonBody* body, float, int)
{
    NewtonNode* core = (NewtonNode*)NewtonBodyGetUserData(body);

    if(!core)
        throw tbe::Exception("NewtonNode::ApplyForceAndTorque; core == NULL");

    if(core->m_applyGravity)
        core->m_applyForce.y -= core->m_masse * 9.81 * core->m_parallelScene->GetGravity();

    NewtonBodySetForce(body, core->m_applyForce);
    NewtonBodySetTorque(body, core->m_applyTorque);

    core->m_applyForce = 0;
}
