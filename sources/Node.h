#ifndef _NODE_H
#define _NODE_H

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "Mathematics.h"
#include "Any.h"
#include "AABB.h"
#include "Texture.h"
#include "Exception.h"
#include "Iterator.h"

namespace tbe
{
namespace scene
{

class ParallelScene;
class SceneManager;

/**
 * \brief Class de base pour la représentation d'une entité
 * qui inclus une matrice, un AABB et un nom
 */
class Node
{
public:
    Node();
    Node(const Node& copy);
    virtual ~Node();

    virtual Node & operator=(const Node& copy);

    /// Operateur de comparaison par nom (m_name)
    bool operator==(Node* node) const;
    bool operator==(std::string name) const;

    /// Renvois le AABB du noeud
    AABB GetAabb() const;
    AABB GetAbsolutAabb() const;

    /// Specifier si l'objet est activer
    void SetEnable(bool enbale);
    bool IsEnable() const;

    /// Identifiant du noeud
    void SetName(std::string name);
    std::string GetName() const;

    /// Matrice du noeud
    void SetMatrix(const Matrix4f& matrix);
    void MulMatrix(const Matrix4f& matrix);

    Matrix4f GetAbsoluteMatrix() const;
    Matrix4f& GetMatrix();

    /// Spécifier la scene parallel parent
    void SetParallelScene(ParallelScene* parallelScene);
    ParallelScene* GetParallelScene() const;

    void SetSceneManager(SceneManager* sceneManager);
    SceneManager* GetSceneManager() const;

    /// Racourcie pour sépcifier la postion de la matrice du noeud
    void SetPos(Vector3f pos);
    Vector3f GetPos() const;

    Vector3f MapFromGlobal(Vector3f pos);

    bool IsRoot() const;
    bool IsAttached() const;
    void ReleaseParent();

    unsigned DeepPosition() const;

    void SetParent(Node* parent);
    Node* GetParent() const;

    void AddChild(Node* child);
    Node* GetChild(unsigned index) const;
    unsigned GetChildCount() const;

    void ClearAllChild();

    Iterator<Node*> GetChildIterator();

    void ReleaseChild(Node* child);
    Node* ReleaseChild(unsigned index);

    void DeleteChild(Node* child);
    void DeleteChild(unsigned index);

    virtual Node* Clone() = 0;

    virtual void Process() = 0;

    virtual void Render() = 0;

    void SetUserData(Any userData);
    Any GetUserData() const;

    typedef std::map<std::string, std::string> CtorMap;

    virtual CtorMap ConstructionMap();

    typedef std::map<std::string, Node*> Map;
    typedef std::vector<Node*> Array;

protected:
    ParallelScene* m_parallelScene;
    SceneManager* m_sceneManager;
    std::string m_name;
    Matrix4f m_matrix;
    bool m_enable;
    AABB m_aabb;

    Node* m_parent;
    Node::Array m_childs;

    Any m_userData;
};

class BullNode : public Node
{

    Node* Clone()
    {
        return new BullNode(*this);
    }

    void Process()
    {
        if(!m_enable)
            return;

        for_each(m_childs.begin(), m_childs.end(), std::mem_fun(&Node::Process));
    }

    void Render()
    {
        // Nothging to do...
    }
};

}
}

#endif // _NODE_H
