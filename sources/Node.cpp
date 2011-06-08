/*
 * File:   Node.cpp
 * Author: b4n92uid
 *
 * Created on 14 juillet 2010, 16:11:27
 */

#include "Node.h"
#include "ParallelScene.h"
#include "SceneManager.h"
#include "Tools.h"

using namespace tbe;
using namespace tbe::scene;
using namespace std;

// Node ------------------------------------------------------------------------

Node::Node()
{
    m_enable = true;
    m_parallelScene = NULL;
    m_parent = NULL;
}

Node::Node(const Node& copy)
{
    m_enable = true;
    m_parallelScene = NULL;
    m_parent = NULL;

    this->copy(copy);
}

Node::~Node()
{
    clearAllChild();

    if(m_parent)
        m_parent->releaseChild(this);
}

Node& Node::operator =(const Node& copy)
{
    return this->copy(copy);
}

Node& Node::copy(const Node& copy)
{
    m_name = copy.m_name;
    m_matrix = copy.m_matrix;
    m_enable = copy.m_enable;
    m_aabb = copy.m_aabb;
    m_sceneManager = copy.m_sceneManager;

    clearAllChild();

    m_childs.reserve(copy.m_childs.size());

    for(unsigned i = 0; i < copy.m_childs.size(); i++)
        addChild(copy.m_childs[i]->clone());

    return *this;
}

bool Node::operator==(std::string name) const
{
    return m_name == name;
}

bool Node::operator==(Node* node) const
{
    return m_name == node->m_name;
}

void Node::setPos(Vector3f pos)
{
    m_matrix.setPos(pos);
}

Vector3f Node::getPos() const
{
    return m_matrix.getPos();
}

Vector3f Node::mapFromGlobal(Vector3f pos)
{
    if(m_parent)
        return m_parent->mapFromGlobal(m_parent->getMatrix() * pos);
    else
        return pos;
}

void Node::mulMatrix(const Matrix4& matrix)
{
    this->m_matrix *= matrix;
}

void Node::setMatrix(const Matrix4& matrix)
{
    this->m_matrix = matrix;
}

Matrix4 Node::getAbsoluteMatrix() const
{
    return m_parent ? m_matrix * m_parent->getAbsoluteMatrix() : m_matrix;
}

Matrix4& Node::getMatrix()
{
    return m_matrix;
}

void Node::setParallelScene(ParallelScene* parallelScene)
{
    this->m_parallelScene = parallelScene;
}

ParallelScene* Node::getParallelScene() const
{
    return m_parallelScene;
}

AABB Node::getAbsolutAabb() const
{
    Vector3f pos = getAbsoluteMatrix().getPos();
    return AABB(pos + m_aabb.min, pos + m_aabb.max);
}

AABB Node::getAabb() const
{
    return m_aabb;
}

void Node::setEnable(bool enbale)
{
    this->m_enable = enbale;
}

bool Node::isEnable() const
{
    return m_enable;
}

void Node::setName(std::string name)
{
    this->m_name = name;
}

std::string Node::getName() const
{
    return m_name;
}

bool Node::isParent(Node* searche) const
{
    const Node* it = m_parent;

    while(it)
    {
        if(it == searche)
            return true;
        else
            it = it->m_parent;
    }

    return false;
}

bool Node::isChild(Node* searche, bool recursiv) const
{
    for(unsigned i = 0; i < m_childs.size(); i++)
    {
        if(m_childs[i] == searche)
            return true;

        if(recursiv)
            return m_childs[i]->isChild(searche, recursiv);
    }

    return false;
}

bool Node::isRoot() const
{
    if(m_sceneManager)
        return (this == m_sceneManager->getRootNode());
    else
        return false;
}

bool Node::isAttached() const
{
    return m_parent;
}

void Node::releaseFromParent()
{
    if(!m_parent)
        return;

    m_parent->releaseChild(this);
    m_parent = NULL;
}

unsigned Node::deepPosition() const
{
    unsigned deep = 0;

    const Node* it = this;

    do
    {
        it = it->m_parent;
        if(it) deep++;
    }
    while(it);

    return deep;
}

void Node::setParent(Node* parent)
{
    parent->addChild(this);
}

Node* Node::getParent() const
{
    return m_parent;
}

void Node::addChild(Node* child)
{
    if(find(m_childs.begin(), m_childs.end(), child) != m_childs.end())
        throw Exception("Node::AddChild; child already exist");

    if(child->m_parent)
        child->m_parent->releaseChild(child);

    child->m_parent = this;
    child->m_sceneManager = m_sceneManager;

    m_childs.push_back(child);
}

void Node::releaseChild(Node* child)
{
    Node::Array::iterator it = find(m_childs.begin(), m_childs.end(), child);

    if(it == m_childs.end())
        throw Exception("Node::ReleaseChild; cannot found child");

    (*it)->m_parent = NULL;

    m_childs.erase(it);
}

Node* Node::releaseChild(unsigned index)
{
    if(index >= m_childs.size())
        throw Exception("Node::ReleaseChild; index out of range %d", index);

    Node* ret = m_childs[index];
    ret->releaseFromParent();

    m_childs.erase(m_childs.begin() + index);

    return ret;
}

void Node::deleteChild(Node* child)
{
    Node::Array::iterator it = find(m_childs.begin(), m_childs.end(), child);

    if(it == m_childs.end())
        throw Exception("Node::DeleteChild; cannot found child");

    delete (*it);
    m_childs.erase(it);
}

void Node::deleteChild(unsigned index)
{
    if(index >= m_childs.size())
        throw Exception("Node::DeleteChild; index out of range %d", index);

    delete m_childs[index];
    m_childs.erase(m_childs.begin() + index);
}

unsigned Node::getChildCount() const
{
    return m_childs.size();
}

Node* Node::getChild(unsigned index) const
{
    if(index >= m_childs.size())
        throw Exception("Node::GetChild; index out of range %d", index);

    return m_childs[index];
}

const Any::Map Node::getUserDatas() const
{
    return m_userDatas;
}

void Node::setUserData(std::string key, Any value)
{
    m_userDatas[key] = value;
}

void Node::clearUserData()
{
    m_userDatas.clear();
}

void Node::delUserData(std::string key)
{
    if(m_userDatas.count(key))
        m_userDatas.erase(key);
}

bool Node::hasUserData(std::string key) const
{
    return m_userDatas.find(key) != m_userDatas.end();
}

Any Node::getUserData(std::string key) const
{
    map<string, Any>::const_iterator it = m_userDatas.find(key);

    if(it == m_userDatas.end())
        throw Exception("Node::getUserData; undifined key %s", key.c_str());
    else
        return it->second;
}

Iterator<Node*> Node::getChildIterator()
{
    return Iterator<Node*>(m_childs);
}

void Node::clearAllChild()
{
    // Copie de pointeur pour éviter les problemes d'itération
    // lors de la suppresion

    for(unsigned i = 0; i < m_childs.size(); i++)
        delete m_childs[i];

    m_childs.clear();
}

Node::CtorMap Node::constructionMap(std::string root)
{
    Node::CtorMap ctormap;

    ctormap["name"] = m_name;
    ctormap["matrix"] = tools::mat4ToStr(m_matrix);

    for(Any::Map::iterator it = m_userDatas.begin(); it != m_userDatas.end(); it++)
        ctormap["." + it->first] = it->second.getValue<string > ();

    return ctormap;
}

void Node::setSceneManager(SceneManager* sceneManager)
{
    this->m_sceneManager = sceneManager;
}

SceneManager* Node::getSceneManager() const
{
    return m_sceneManager;
}
