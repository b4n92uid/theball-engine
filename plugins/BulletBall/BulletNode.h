/*
 * File:   BulletNode.h
 * Author: b4n92uid
 *
 * Created on 08 decembre 2012, 00:00
 */

#ifndef _BULLET_NODE_H
#define	_BULLET_NODE_H

#include <btBulletDynamicsCommon.h>

#define tbe2btVec(v) btVector3(v.x, v.y, v.z)
#define bt2tbeVec(v) tbe::Vector3f(v.getX(), v.getY(), v.getZ())

namespace tbe
{
namespace scene
{

class BulletParallelScene;

/// \brief Class represantant un objet physique dans l'espace

class BulletNode : public Node
{
public:
    BulletNode(BulletParallelScene* newtonScene);
    BulletNode(BulletParallelScene* newtonScene, Node* node);
    BulletNode(BulletParallelScene* newtonScene, Matrix4* matrix);
    BulletNode(const BulletNode& copy);

    virtual ~BulletNode();

    bool operator=(const BulletNode& copy);

    void buildBoxNode(Vector3f size, float masse);
    void buildSphereNode(Vector3f size, float masse);
    void buildCylinderNode(Vector3f size, float masse);
    void buildConvexNode(const Vertex::Array& vertexes, float masse);
    void buildConvexNode(const tbe::scene::Mesh* mesh, float masse);
    void buildTreeNode(const Face::Array& faces);
    void buildTreeNode(const tbe::scene::Mesh* mesh);

    /// Destruction de l'objet physique
    void destroyBody();

    /// Recuperation Objet NewtonBody
    btRigidBody* getBody() const;

    /// Masse
    float getMasse();

    /// Assigne une matrice
    void setMatrix(Matrix4 matrix);
    Matrix4 getMatrix();

    void setPos(Vector3f pos);
    Vector3f getPos();

    void setVelocity(Vector3f vel);
    Vector3f getVelocity();

    void setOmega(Vector3f vel);
    Vector3f getOmega();

    void setGravity(bool enable);

    /// Mise a jour
    void render();

    void process();

    Node* clone();

    void setUpdatedMatrix(Matrix4* updatedMatrix);
    Matrix4* getUpdatedMatrix() const;

    void setApplyTorque(Vector3f applyTorque);
    Vector3f getApplyTorque();

    void setApplyForce(Vector3f applyForce);
    Vector3f getApplyForce();

    bool isCollidWith(BulletNode* node) const;

    BulletParallelScene* getParallelScene() const;

    typedef std::vector<BulletNode*> Array;
    typedef std::map<std::string, BulletNode*> Map;

protected:
    BulletParallelScene* m_parallelScene;

    btTransform m_transform;

    btDiscreteDynamicsWorld* m_world;
    btMotionState *m_motionState;
    btCollisionShape* m_shape;
    btRigidBody* m_body;

    Matrix4* m_updatedMatrix;

    float m_masse;
    bool m_applyGravity;
};

class NodeMotionState : public btMotionState
{
public:

    NodeMotionState(Matrix4* matrix)
    {
        m_matrix = matrix;
    }

    virtual ~NodeMotionState()
    {
    }

    virtual void getWorldTransform(btTransform &worldTrans) const
    {
        worldTrans.setFromOpenGLMatrix(*m_matrix);
    }

    virtual void setWorldTransform(const btTransform &worldTrans)
    {
        if(!m_matrix)
            return;

        worldTrans.getOpenGLMatrix(*m_matrix);
    }

protected:
    Matrix4* m_matrix;
};

}
}

#endif	/* _BULLET_NODE_H */

