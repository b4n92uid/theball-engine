/* 
 * File:   NewtonNode.h
 * Author: b4n92uid
 *
 * Created on 27 novembre 2009, 19:35
 */

#ifndef _NEWTONNODE_H
#define	_NEWTONNODE_H

#include <Newton.h>

namespace tbe
{
namespace scene
{

class NewtonParallelScene;

/// \brief Class represantant un objet physique dans l'espace

class NewtonNode : public Node
{
public:
    NewtonNode(NewtonParallelScene* newtonScene);
    NewtonNode(NewtonParallelScene* newtonScene, Node* node);
    NewtonNode(NewtonParallelScene* newtonScene, Matrix4f* matrix);
    NewtonNode(const NewtonNode& copy);

    virtual ~NewtonNode();

    bool operator=(const NewtonNode& copy);

    void BuildBoxNode(Vector3f size, float masse);
    void BuildSphereNode(Vector3f size, float masse);
    void BuildCylinderNode(Vector3f size, float masse);
    void BuildConvexNode(const Vertex::Array& vertexes, float masse);
    void BuildTreeNode(const Face::Array& faces);

    /// Destruction de l'objet physique
    void DestroyBody();

    /// Recuperation Objet NewtonBody
    NewtonBody* GetBody();

    /// Test de collision
    bool IsCollidWith(const NewtonNode* target);

    /// Masse
    float GetMasse();

    /// Assigne une matrice
    void SetMatrix(Matrix4f matrix);
    Matrix4f GetMatrix();

    void SetPos(Vector3f pos);
    Vector3f GetPos();

    void SetVelocity(Vector3f vel);
    Vector3f GetVelocity();

    /// Mise a jour
    void Render();

    void Process();

    Node* Clone();

    void SetUpdatedMatrix(Matrix4f* updatedMatrix);
    Matrix4f* GetUpdatedMatrix() const;

    void SetApplyTorque(Vector3f applyTorque);
    Vector3f GetApplyTorque() const;

    void SetApplyForce(Vector3f applyForce);
    Vector3f GetApplyForce() const;

    void SetApplyGravity(bool applyGravity);
    bool IsApplyGravity() const;

    void SetFreeze(bool freeze);
    bool IsFreeze() const;

    NewtonParallelScene* GetParallelScene() const;
    
    /// Method static d'application de la force gravitationnele (9.81)
    static void ApplyForceAndTorque(const NewtonBody* body, float, int);

    typedef std::vector<NewtonNode*> Array;
    typedef std::map<std::string, NewtonNode*> Map;

protected:
    Vector3f m_applyForce;
    Vector3f m_applyTorque;
    NewtonParallelScene* m_parallelScene;
    NewtonWorld* m_newtonWorld;
    NewtonBody* m_body;
    Matrix4f* m_updatedMatrix;
    float m_masse;
    bool m_freeze;
    bool m_applyGravity;
};

}
}

#endif	/* _NEWTONNODE_H */

