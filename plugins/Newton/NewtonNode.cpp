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

NewtonNode::NewtonNode()
{
    m_newtonScene = NULL;
    m_newtonWorld = NULL;
    m_body = NULL;

    m_updatedMatrix = NULL;

    m_masse = 0;

    m_freeze = false;
    m_applyGravity = true;
}

NewtonNode::NewtonNode(NewtonParallelScene* newtonScene, Node* node)
{
    m_newtonScene = newtonScene;
    m_newtonWorld = newtonScene->GetNewtonWorld();
    m_body = NULL;

    m_updatedMatrix = &node->GetMatrix();

    m_masse = 0;

    m_freeze = false;
    m_applyGravity = true;
}

NewtonNode::NewtonNode(NewtonParallelScene* newtonScene, Matrix4f* matrix)
{
    m_newtonScene = newtonScene;
    m_newtonWorld = newtonScene->GetNewtonWorld();
    m_body = NULL;

    m_updatedMatrix = matrix;

    m_masse = 0;

    m_freeze = false;
    m_applyGravity = true;
}

NewtonNode::~NewtonNode()
{
    DestroyBody();
}

void NewtonNode::BuildBoxNode(Vector3f size, float masse)
{
    if(!m_newtonWorld)
        throw Exception("NewtonNode::BuildBoxNode; newtonWorld = NULL");

    m_masse = masse;

    size *= 2;

    // Corp de collision
    NewtonCollision* collision = NewtonCreateBox(m_newtonWorld, size.x, size.y, size.z, 0, NULL);
    m_body = NewtonCreateBody(m_newtonWorld, collision);

    // Masse & Inertia & Inertia
    Vector3f origine;
    Vector3f inertia;

    NewtonConvexCollisionCalculateInertialMatrix(collision, inertia, origine);
    inertia *= m_masse;

    NewtonBodySetMassMatrix(m_body, m_masse, inertia.x, inertia.y, inertia.z);

    // Donneés utilisateur
    NewtonBodySetUserData(m_body, this);

    // Callback
    NewtonBodySetForceAndTorqueCallback(m_body, NewtonParallelScene::ApplyForceAndTorque);

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
    m_body = NewtonCreateBody(m_newtonWorld, collision);

    // Masse & Inertia
    Vector3f origine;
    Vector3f inertia;

    NewtonConvexCollisionCalculateInertialMatrix(collision, inertia, origine);
    inertia *= m_masse;

    NewtonBodySetMassMatrix(m_body, m_masse, inertia.x, inertia.y, inertia.z);

    // Donneés utilisateur
    NewtonBodySetUserData(m_body, this);

    // Callback
    NewtonBodySetForceAndTorqueCallback(m_body, NewtonParallelScene::ApplyForceAndTorque);

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
    m_body = NewtonCreateBody(m_newtonWorld, collision);

    // Masse & Inertia
    Vector3f origine;
    Vector3f inertia;

    NewtonConvexCollisionCalculateInertialMatrix(collision, inertia, origine);
    inertia *= m_masse;

    NewtonBodySetMassMatrix(m_body, m_masse, inertia.x, inertia.y, inertia.z);

    // Donneés utilisateur
    NewtonBodySetUserData(m_body, this);

    // Callback
    NewtonBodySetForceAndTorqueCallback(m_body, NewtonParallelScene::ApplyForceAndTorque);

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
    m_body = NewtonCreateBody(m_newtonWorld, collision);

    // Masse & Inertia
    Vector3f origine;
    Vector3f inertia;

    NewtonConvexCollisionCalculateInertialMatrix(collision, inertia, origine);
    inertia *= m_masse;

    NewtonBodySetMassMatrix(m_body, m_masse, inertia.x, inertia.y, inertia.z);

    // Donne utilisateur
    NewtonBodySetUserData(m_body, this);

    // Callback
    NewtonBodySetForceAndTorqueCallback(m_body, NewtonParallelScene::ApplyForceAndTorque);

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
    m_body = NewtonCreateBody(m_newtonWorld, nCollision);
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

void NewtonNode::UpdateMatrix()
{
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

NewtonBody* NewtonNode::GetBody()
{
    return m_body;
}

float NewtonNode::GetMasse()
{
    return m_masse;
}

void NewtonNode::SetNewtonWorld(NewtonWorld* newtonWorld)
{
    this->m_newtonWorld = newtonWorld;
    m_newtonScene = static_cast<NewtonParallelScene*>(NewtonWorldGetUserData(m_newtonWorld));
}

NewtonWorld* NewtonNode::GetNewtonWorld() const
{
    return m_newtonWorld;
}

void NewtonNode::SetNewtonScene(NewtonParallelScene* newtonNodeScene)
{
    this->m_newtonScene = newtonNodeScene;
    m_newtonWorld = m_newtonScene->GetNewtonWorld();
}

NewtonParallelScene* NewtonNode::GetNewtonScene() const
{
    return m_newtonScene;
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

    NewtonBodySetForceAndTorqueCallback(m_body, freeze ? NULL : NewtonParallelScene::ApplyForceAndTorque);
    NewtonBodySetVelocity(m_body, Vector3f(0));
}

bool NewtonNode::IsFreeze() const
{
    return m_freeze;
}
