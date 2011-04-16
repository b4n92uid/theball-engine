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
    AABB getAabb() const;

    /// Renvois le AABB du noeud avec des coordonées sur le repere global
    AABB getAbsolutAabb() const;

    /// Specifier si l'objet est activer
    void setEnable(bool enbale);

    /// Etat de l'objet si activer
    bool isEnable() const;

    /// Identifiant du noeud
    void setName(std::string name);
    std::string getName() const;

    /// Matrice du noeud
    void setMatrix(const Matrix4f& matrix);
    Matrix4f& getMatrix();

    void mulMatrix(const Matrix4f& matrix);

    Matrix4f getAbsoluteMatrix() const;

    /// Spécifier la scene parallel parent
    void setParallelScene(ParallelScene* parallelScene);
    ParallelScene* getParallelScene() const;

    void setSceneManager(SceneManager* sceneManager);
    SceneManager* getSceneManager() const;

    /// Racourcie pour sépcifier la postion de la matrice du noeud
    void setPos(Vector3f pos);
    Vector3f getPos() const;

    Vector3f mapFromGlobal(Vector3f pos);

    bool isRoot() const;
    bool isAttached() const;

    void releaseFromParent();

    unsigned deepPosition() const;

    void setParent(Node* parent);
    Node* getParent() const;

    void addChild(Node* child);
    Node* getChild(unsigned index) const;
    unsigned getChildCount() const;

    void clearAllChild();

    Iterator<Node*> getChildIterator();

    void releaseChild(Node* child);
    Node* releaseChild(unsigned index);

    void deleteChild(Node* child);
    void deleteChild(unsigned index);

    virtual Node* clone() = 0;

    virtual void process() = 0;

    virtual void render() = 0;

    void setUserData(Any userData);
    Any getUserData() const;

    typedef std::map<std::string, std::string> CtorMap;

    virtual CtorMap constructionMap(std::string root);

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

    Node* clone()
    {
        return new BullNode(*this);
    }

    void process()
    {
        if(!m_enable)
            return;

        for_each(m_childs.begin(), m_childs.end(), std::mem_fun(&Node::process));
    }

    void render()
    {
        // Nothging to do...
    }

    Node::CtorMap constructionMap(std::string root)
    {
        Node::CtorMap ctormap = Node::constructionMap(root);

        ctormap["class"] = "BullNode";

        return ctormap;
    }
};

}
}

#endif // _NODE_H
