/*
 * File:   BulletNode.cpp
 * Author: b4n92uid
 *
 * Created on 08 decembre 2012, 00:00
 */

#include <Tbe.h>
#include "BulletNode.h"
#include "BulletParallelScene.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

//// BulletNode ----------------------------------------------------------------

BulletNode::BulletNode(BulletParallelScene* bulletScene)
{
    m_parallelScene = bulletScene;
    m_world = bulletScene->getWorld();
    m_body = NULL;

    m_updatedMatrix = NULL;

    m_masse = 0;

    m_applyGravity = true;

    m_parallelScene->registerNode(this);
}

BulletNode::BulletNode(BulletParallelScene* bulletScene, Node* node)
{
    m_parallelScene = bulletScene;
    m_world = bulletScene->getWorld();
    m_body = NULL;

    m_updatedMatrix = &node->getMatrix();

    m_masse = 0;

    m_applyGravity = true;

    m_parallelScene->registerNode(this);
}

BulletNode::BulletNode(BulletParallelScene* bulletScene, Matrix4* matrix)
{
    m_parallelScene = bulletScene;
    m_world = bulletScene->getWorld();
    m_body = NULL;

    m_updatedMatrix = matrix;

    m_masse = 0;

    m_applyGravity = true;

    m_parallelScene->registerNode(this);
}

BulletNode::BulletNode(const BulletNode& copy)
{
    *this = copy;

    m_parallelScene->registerNode(this);
}

BulletNode::~BulletNode()
{
    destroyBody();

    m_parallelScene->unregisterNode(this);
}

bool BulletNode::operator=(const BulletNode& copy)
{
    m_parallelScene = copy.m_parallelScene;
    m_world = copy.m_world;

    m_masse = copy.m_masse;
    m_applyGravity = copy.m_applyGravity;

    return true;
}

Node* BulletNode::clone()
{
    return new BulletNode(*this);
}

void BulletNode::buildBoxNode(Vector3f size, float masse)
{
    if(!m_world)
        throw Exception("BulletNode::BuildBoxNode; m_world = NULL");

    m_masse = masse;

    // Corp de collision
    m_shape = new btBoxShape(btVector3(size.x, size.y, size.z));

    // Inertia
    btVector3 localInertia(0, 0, 0);

    if(m_masse)
        m_shape->calculateLocalInertia(m_masse, localInertia);

    // Motion
    m_motionState = new NodeMotionState(m_updatedMatrix);

    // Body
    btRigidBody::btRigidBodyConstructionInfo myBoxRigidBodyConstructionInfo(m_masse, m_motionState, m_shape, localInertia);

    m_body = new btRigidBody(myBoxRigidBodyConstructionInfo);
    m_body->setUserPointer(this);

    m_world->addRigidBody(m_body);
}

void BulletNode::buildSphereNode(Vector3f size, float masse)
{
    if(!m_world)
        throw Exception("BulletNode::BuildBoxNode; m_world = NULL");

    m_masse = masse;

    // Corp de collision
    m_shape = new btSphereShape(size.x);

    // Inertia
    btVector3 localInertia(0, 0, 0);

    if(m_masse)
        m_shape->calculateLocalInertia(m_masse, localInertia);

    // Motion
    m_motionState = new NodeMotionState(m_updatedMatrix);

    // Body
    btRigidBody::btRigidBodyConstructionInfo myBoxRigidBodyConstructionInfo(m_masse, m_motionState, m_shape, localInertia);

    m_body = new btRigidBody(myBoxRigidBodyConstructionInfo);
    m_body->setUserPointer(this);

    m_world->addRigidBody(m_body);
}

void BulletNode::buildCylinderNode(Vector3f size, float masse)
{
    if(!m_world)
        throw Exception("BulletNode::BuildBoxNode; m_world = NULL");

    m_masse = masse;

    // Corp de collision
    m_shape = new btCylinderShape(btVector3(size.x, size.y, size.z));

    // Inertia
    btVector3 localInertia(0, 0, 0);

    if(m_masse)
        m_shape->calculateLocalInertia(m_masse, localInertia);

    // Motion
    m_motionState = new NodeMotionState(m_updatedMatrix);

    // Body
    btRigidBody::btRigidBodyConstructionInfo myBoxRigidBodyConstructionInfo(m_masse, m_motionState, m_shape, localInertia);

    m_body = new btRigidBody(myBoxRigidBodyConstructionInfo);
    m_body->setUserPointer(this);

    m_world->addRigidBody(m_body);
}

inline Vector3f::Array ExtractPos(const Vertex::Array& vertexes)
{
    Vector3f::Array onlyPos;

    for(unsigned i = 0; i < vertexes.size(); i++)
        onlyPos.push_back(vertexes[i].pos);

    return onlyPos;
}

void BulletNode::buildConvexNode(const tbe::scene::Mesh* mesh, float masse)
{
    Vertex::Array vertexes = mesh->getHardwareBuffer()->getAllVertex(true);

    const Vector3f scale = mesh->getMatrix().decompose().scale;

    for(unsigned i = 0; i < vertexes.size(); i++)
        vertexes[i].pos *= scale;

    buildConvexNode(vertexes, masse);
}

void BulletNode::buildConvexNode(const Vertex::Array& vertexes, float masse)
{
    if(!m_world)
        throw Exception("BulletNode::BuildBoxNode; newtonWorld = NULL");

    m_masse = masse;

    Vector3f::Array onlyPos = ExtractPos(vertexes);

    // Corp de collision
    m_shape = new btConvexHullShape(&onlyPos[0].x, vertexes.size(), sizeof(Vector3f));

    // Inertia
    btVector3 localInertia(0, 0, 0);

    if(m_masse)
        m_shape->calculateLocalInertia(m_masse, localInertia);

    // Motion
    m_motionState = new NodeMotionState(m_updatedMatrix);

    // Body
    btRigidBody::btRigidBodyConstructionInfo myBoxRigidBodyConstructionInfo(m_masse, m_motionState, m_shape, localInertia);

    m_body = new btRigidBody(myBoxRigidBodyConstructionInfo);
    m_body->setUserPointer(this);

    m_world->addRigidBody(m_body);
}

void BulletNode::buildTreeNode(const tbe::scene::Mesh* mesh)
{
    Face::Array faces = mesh->getHardwareBuffer()->getAllFace();

    Vector3f scale = mesh->getMatrix().decompose().scale;

    for(unsigned i = 0; i < faces.size(); i++)
        for(unsigned j = 0; j < faces[i].size(); j++)
            faces[i][j].pos *= scale;

    buildTreeNode(faces);
}

void BulletNode::buildTreeNode(const Face::Array& faces)
{
    if(!m_world)
        throw Exception("BulletNode::BuildBoxNode; m_world = NULL");

    m_masse = 0;

    // Corp de collision
    btTriangleMesh* data = new btTriangleMesh();

    for(unsigned int i = 0; i < faces.size(); i++)
    {
        btVector3 A = tbe2btVec(faces[i][0].pos);
        btVector3 B = tbe2btVec(faces[i][1].pos);
        btVector3 C = tbe2btVec(faces[i][2].pos);

        data->addTriangle(A, B, C, false); // false, don't remove duplicate vertices
    }

    // true for using quantization; true for building the BVH
    m_shape = new btBvhTriangleMeshShape(data, true, true);

    // Motion
    m_motionState = new NodeMotionState(m_updatedMatrix);

    // Body
    btRigidBody::btRigidBodyConstructionInfo myBoxRigidBodyConstructionInfo(m_masse, m_motionState, m_shape, btVector3(0, 0, 0));

    m_body = new btRigidBody(myBoxRigidBodyConstructionInfo);
    m_body->setUserPointer(this);

    m_world->addRigidBody(m_body);
}

void BulletNode::destroyBody()
{
    if(m_body)
    {
        m_world->removeRigidBody(m_body);

        delete m_shape, m_shape = NULL;
        delete m_motionState, m_motionState = NULL;
        delete m_body, m_body = NULL;

        m_masse = 0;
    }
}

void BulletNode::render()
{
}

void BulletNode::process()
{
    if(!m_enable)
        return;

    for(unsigned i = 0; i < m_childs.size(); i++)
        m_childs[i]->process();
}

void BulletNode::setMatrix(Matrix4 matrix)
{
    if(!m_body)
        return;

    btTransform tr;
    tr.setFromOpenGLMatrix(*m_updatedMatrix);

    m_body->setWorldTransform(tr);
}

Matrix4 BulletNode::getMatrix()
{
    return *m_updatedMatrix;
}

void BulletNode::setPos(Vector3f pos)
{
    if(!m_body)
        return;

    btTransform tr;
    tr.setOrigin(tbe2btVec(pos));

    m_body->setWorldTransform(tr);
}

Vector3f BulletNode::getPos()
{
    return m_updatedMatrix->getPos();
}

void BulletNode::setOmega(Vector3f vel)
{
    m_body->setAngularVelocity(tbe2btVec(vel));
}

Vector3f BulletNode::getOmega()
{
    return bt2tbeVec(m_body->getAngularVelocity());
}

void BulletNode::setVelocity(Vector3f vel)
{
    m_body->setLinearVelocity(tbe2btVec(vel));
}

Vector3f BulletNode::getVelocity()
{
    return bt2tbeVec(m_body->getLinearVelocity());
}

btRigidBody* BulletNode::getBody() const
{
    return m_body;
}

float BulletNode::getMasse()
{
    return m_masse;
}

void BulletNode::setUpdatedMatrix(Matrix4* updatedMatrix)
{
    this->m_updatedMatrix = updatedMatrix;
}

Matrix4* BulletNode::getUpdatedMatrix() const
{
    return m_updatedMatrix;
}

void BulletNode::setApplyTorque(Vector3f applyTorque)
{
    m_body->applyTorque(tbe2btVec(applyTorque));
}

Vector3f BulletNode::getApplyTorque()
{
    return bt2tbeVec(m_body->getTotalTorque());
}

void BulletNode::setApplyForce(Vector3f applyForce)
{
    m_body->applyCentralForce(tbe2btVec(applyForce));
}

Vector3f BulletNode::getApplyForce()
{
    return bt2tbeVec(m_body->getTotalForce());
}

void BulletNode::setGravity(bool enable)
{
    if(enable)
        m_body->setFlags(0);
    else
        m_body->setFlags(BT_DISABLE_WORLD_GRAVITY);
}

BulletParallelScene* BulletNode::getParallelScene() const
{
    return m_parallelScene;
}

bool BulletNode::isCollidWith(BulletNode* node) const
{
    int numManifolds = m_world->getDispatcher()->getNumManifolds();

    for(int i = 0; i < numManifolds; i++)
    {
        btPersistentManifold* contactManifold = m_world->getDispatcher()->getManifoldByIndexInternal(i);
        const btCollisionObject* obA = contactManifold->getBody0();
        const btCollisionObject* obB = contactManifold->getBody1();

        if(obA == this->m_body && obB == node->m_body
           || obA == node->m_body && obB == this->m_body)
            return true;
    }

    return false;
}
