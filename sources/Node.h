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
    void setMatrix(const Matrix4& matrix);
    void mulMatrix(const Matrix4& matrix);

    void setScale(Vector3f scale);
    Vector3f getScale() const;

    void setRotation(Quaternion rotation);
    Quaternion getRotation() const;

    void setPos(Vector3f pos);
    Vector3f getPos() const;

    Matrix4& getMatrix();
    Matrix4 getMatrix() const;

    Matrix4 getAbsoluteMatrix(bool includeThis = true) const;

    /// Spécifier la scene parallel parent
    void setParallelScene(ParallelScene* parallelScene);
    ParallelScene* getParallelScene() const;

    void setSceneManager(SceneManager* sceneManager);
    SceneManager* getSceneManager() const;


    Vector3f mapToGlobal(Vector3f pos);

    bool isParent(Node* searche) const;
    bool isChild(Node* searche, bool recursiv = false) const;

    bool isRoot() const;
    bool isAttached() const;

    void releaseFromParent();

    unsigned deepPosition() const;

    void dettach();

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

    virtual void setup();

    virtual Node* clone() = 0;

    virtual void process() = 0;

    virtual void render() = 0;

    void setUserData(std::string key, Any value);
    Any getUserData(std::string key) const;

    void delUserData(std::string key);

    void clearUserData();

    bool hasUserData(std::string key) const;

    const Any::Map getUserDatas() const;

    typedef std::map<std::string, std::string> CtorMap;

    void addToConstructionMap(std::string name, std::string value);

    virtual CtorMap constructionMap(std::string root);

    virtual std::vector<std::string> getUsedRessources();

    typedef std::map<std::string, Node*> Map;
    typedef std::vector<Node*> Array;

protected:
    ParallelScene* m_parallelScene;
    SceneManager* m_sceneManager;
    std::string m_name;
    bool m_enable;
    AABB m_aabb;

    Vector3f m_position;
    Quaternion m_rotation;
    Vector3f m_scale;
    Matrix4 m_matrix;

    Node* m_parent;
    Node::Array m_childs;

    Any::Map m_userDatas;

    CtorMap m_addtionalCtorMap;

private:
    Node& copy(const Node& copy);
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
