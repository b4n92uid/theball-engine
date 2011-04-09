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

void Node::setSceneManager(SceneManager* sceneManager)
{
    this->m_sceneManager = sceneManager;
}

SceneManager* Node::getSceneManager() const
{
    return m_sceneManager;
}

Node::Node(const Node& copy)
{
    m_parent = NULL;

    *this = copy;
}

Node::~Node()
{
    clearAllChild();

    if(m_parent)
        m_parent->releaseChild(this);
}

Node& Node::operator =(const Node& copy)
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
        return m_parent->mapFromGlobal(pos - m_parent->getPos());
    else
        return pos;
}

void Node::mulMatrix(const Matrix4f& matrix)
{
    this->m_matrix *= matrix;
}

void Node::setMatrix(const Matrix4f& matrix)
{
    this->m_matrix = matrix;
}

Matrix4f Node::getAbsoluteMatrix() const
{
    return m_parent ? m_parent->getAbsoluteMatrix() * m_matrix : m_matrix;
}

Matrix4f& Node::getMatrix()
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

    m_aabb.count(child->getAbsolutAabb());
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

void Node::setUserData(Any userData)
{
    this->m_userData = userData;
}

Any Node::getUserData() const
{
    return m_userData;
}

Iterator<Node*> Node::getChildIterator()
{
    return Iterator<Node*>(m_childs);
}

void Node::clearAllChild()
{
    // Copie de pointeur pour �viter les probleme d'�tiration
    // lors de la suppr�sion

    Node::Array dtor = m_childs;

    for(unsigned i = 0; i < dtor.size(); i++)
        delete dtor[i];
}

Node::CtorMap Node::constructionMap(std::string root)
{
    Node::CtorMap ctormap;

    ctormap["name"] = m_name;
    ctormap["matrix"] = tools::mat4ToStr(m_matrix);

    return ctormap;
}