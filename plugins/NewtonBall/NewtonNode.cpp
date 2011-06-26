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
    m_newtonWorld = newtonScene->getNewtonWorld();
    m_body = NULL;

    m_updatedMatrix = NULL;

    m_masse = 0;

    m_applyGravity = true;

    m_parallelScene->registerNode(this);
}

NewtonNode::NewtonNode(NewtonParallelScene* newtonScene, Node* node)
{
    m_parallelScene = newtonScene;
    m_newtonWorld = newtonScene->getNewtonWorld();
    m_body = NULL;

    m_updatedMatrix = &node->getMatrix();

    m_masse = 0;

    m_applyGravity = true;

    m_parallelScene->registerNode(this);
}

NewtonNode::NewtonNode(NewtonParallelScene* newtonScene, Matrix4* matrix)
{
    m_parallelScene = newtonScene;
    m_newtonWorld = newtonScene->getNewtonWorld();
    m_body = NULL;

    m_updatedMatrix = matrix;

    m_masse = 0;

    m_applyGravity = true;

    m_parallelScene->registerNode(this);
}

NewtonNode::NewtonNode(const NewtonNode& copy)
{
    *this = copy;

    m_parallelScene->registerNode(this);
}

NewtonNode::~NewtonNode()
{
    destroyBody();

    m_parallelScene->unregisterNode(this);
}

bool NewtonNode::operator=(const NewtonNode& copy)
{
    m_applyForce = copy.m_applyForce;
    m_applyTorque = copy.m_applyTorque;
    m_parallelScene = copy.m_parallelScene;
    m_newtonWorld = copy.m_newtonWorld;

    m_masse = copy.m_masse;
    m_applyGravity = copy.m_applyGravity;

    return true;
}

Node* NewtonNode::clone()
{
    return new NewtonNode(*this);
}

void NewtonNode::buildBoxNode(Vector3f size, float masse)
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
    NewtonBodySetForceAndTorqueCallback(m_body, NewtonNode::applyForceAndTorqueCallback);

    size /= 2;

    // Matrice
    NewtonBodySetMatrix(m_body, *m_updatedMatrix);

    NewtonReleaseCollision(m_newtonWorld, collision);
}

void NewtonNode::buildSphereNode(Vector3f size, float masse)
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
    NewtonBodySetForceAndTorqueCallback(m_body, NewtonNode::applyForceAndTorqueCallback);

    // Matrice
    NewtonBodySetMatrix(m_body, *m_updatedMatrix);

    NewtonReleaseCollision(m_newtonWorld, collision);
}

void NewtonNode::buildCylinderNode(Vector3f size, float masse)
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
    NewtonBodySetForceAndTorqueCallback(m_body, NewtonNode::applyForceAndTorqueCallback);

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

void NewtonNode::buildConvexNode(const Vertex::Array& vertexes, float masse)
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
    NewtonBodySetForceAndTorqueCallback(m_body, NewtonNode::applyForceAndTorqueCallback);

    // Matrice
    NewtonBodySetMatrix(m_body, *m_updatedMatrix);

    NewtonReleaseCollision(m_newtonWorld, collision);
}

void NewtonNode::buildTreeNode(const Face::Array& faces)
{
    if(!m_newtonWorld)
        throw Exception("NewtonNode::BuildBoxNode; newtonWorld = NULL");

    m_masse = 0;

    // Corp de collision

    NewtonCollision* nCollision = NewtonCreateTreeCollision(m_newtonWorld, 0);
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

void NewtonNode::destroyBody()
{
    if(m_body)
    {
        NewtonDestroyBody(m_newtonWorld, m_body);

        m_body = NULL;
        m_masse = 0;
    }
}

bool NewtonNode::isCollidWith(const NewtonNode* target) const
{
    Vector3f contact, normal, penetration;

    return NewtonCollisionCollide(m_parallelScene->getNewtonWorld(), 1,
                                  NewtonBodyGetCollision(m_body), m_matrix,
                                  NewtonBodyGetCollision(target->m_body), target->m_matrix,
                                  contact, normal, penetration, 0);
}

void NewtonNode::render()
{
}

void NewtonNode::process()
{
    if(!m_enable)
        return;

    for(unsigned i = 0; i < m_childs.size(); i++)
        m_childs[i]->process();

    if(m_body)
        NewtonBodyGetMatrix(m_body, *m_updatedMatrix);
}

void NewtonNode::setMatrix(Matrix4 matrix)
{
    *m_updatedMatrix = matrix;

    if(m_body)
        NewtonBodySetMatrix(m_body, *m_updatedMatrix);
}

Matrix4 NewtonNode::getMatrix()
{
    return *m_updatedMatrix;
}

void NewtonNode::setPos(Vector3f pos)
{
    m_updatedMatrix->setPos(pos);

    if(m_body)
        NewtonBodySetMatrix(m_body, *m_updatedMatrix);
}

Vector3f NewtonNode::getPos()
{
    return m_updatedMatrix->getPos();
}

void NewtonNode::setOmega(Vector3f vel)
{
    NewtonBodySetOmega(m_body, vel);
}

Vector3f NewtonNode::getOmega()
{
    Vector3f vel;
    NewtonBodyGetOmega(m_body, vel);
    return vel;
}

void NewtonNode::setVelocity(Vector3f vel)
{
    NewtonBodySetVelocity(m_body, vel);
}

Vector3f NewtonNode::getVelocity()
{
    Vector3f vel;
    NewtonBodyGetVelocity(m_body, vel);
    return vel;
}

NewtonBody* NewtonNode::getBody() const
{
    return m_body;
}

float NewtonNode::getMasse()
{
    return m_masse;
}

void NewtonNode::setUpdatedMatrix(Matrix4* updatedMatrix)
{
    this->m_updatedMatrix = updatedMatrix;
}

Matrix4* NewtonNode::getUpdatedMatrix() const
{
    return m_updatedMatrix;
}

void NewtonNode::setApplyTorque(Vector3f applyTorque)
{
    this->m_applyTorque = applyTorque;
}

Vector3f NewtonNode::getApplyTorque() const
{
    return m_applyTorque;
}

void NewtonNode::setApplyForce(Vector3f applyForce)
{
    this->m_applyForce = applyForce;
}

Vector3f NewtonNode::getApplyForce() const
{
    return m_applyForce;
}

void NewtonNode::setApplyGravity(bool applyGravity)
{
    this->m_applyGravity = applyGravity;
}

bool NewtonNode::isApplyGravity() const
{
    return m_applyGravity;
}

NewtonParallelScene* NewtonNode::getParallelScene() const
{
    return m_parallelScene;
}

void NewtonNode::applyForceAndTorque()
{
    if(m_applyGravity)
        m_applyForce.y -= m_masse * 9.81 * m_parallelScene->getGravity();

    NewtonBodySetForce(m_body, m_applyForce);
    NewtonBodySetTorque(m_body, m_applyTorque);

    m_applyForce = 0;
    m_applyTorque = 0;
}

void NewtonNode::applyForceAndTorqueCallback(const NewtonBody* body, float, int)
{
    NewtonNode* parent = (NewtonNode*)NewtonBodyGetUserData(body);

    if(!parent)
        throw tbe::Exception("NewtonNode::applyForceAndTorqueCallback; core == NULL");

    parent->applyForceAndTorque();
}
