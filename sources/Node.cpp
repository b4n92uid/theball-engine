/*
 * File:   Node.cpp
 * Author: b4n92uid
 *
 * Created on 14 juillet 2010, 16:11:27
 */

#include "Node.h"
#include "ParallelScene.h"
#include "SceneManager.h"

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

void Node::SetSceneManager(SceneManager* sceneManager)
{
    this->m_sceneManager = sceneManager;
}

SceneManager* Node::GetSceneManager() const
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
    ClearAllChild();

    if(m_parent)
        m_parent->ReleaseChild(this);
}

Node& Node::operator =(const Node& copy)
{
    m_name = copy.m_name;
    m_matrix = copy.m_matrix;
    m_enable = copy.m_enable;
    m_aabb = copy.m_aabb;
    m_sceneManager = copy.m_sceneManager;

    ClearAllChild();

    m_childs.reserve(copy.m_childs.size());

    for(unsigned i = 0; i < copy.m_childs.size(); i++)
        AddChild(copy.m_childs[i]->Clone());

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

void Node::SetPos(Vector3f pos)
{
    m_matrix.SetPos(pos);
}

Vector3f Node::GetPos() const
{
    return m_matrix.GetPos();
}

Vector3f Node::MapFromGlobal(Vector3f pos)
{
    if(m_parent)
        return m_parent->MapFromGlobal(pos - m_parent->GetPos());
    else
        return pos;
}

void Node::MulMatrix(const Matrix4f& matrix)
{
    this->m_matrix *= matrix;
}

void Node::SetMatrix(const Matrix4f& matrix)
{
    this->m_matrix = matrix;
}

Matrix4f Node::GetAbsoluteMatrix() const
{
    return m_parent ? m_parent->GetAbsoluteMatrix() * m_matrix : m_matrix;
}

Matrix4f& Node::GetMatrix()
{
    return m_matrix;
}

void Node::SetParallelScene(ParallelScene* parallelScene)
{
    this->m_parallelScene = parallelScene;
}

ParallelScene* Node::GetParallelScene() const
{
    return m_parallelScene;
}

AABB Node::GetAbsolutAabb() const
{
    Vector3f pos = GetAbsoluteMatrix().GetPos();
    return AABB(pos + m_aabb.min, pos + m_aabb.max);
}

AABB Node::GetAabb() const
{
    return m_aabb;
}

void Node::SetEnable(bool enbale)
{
    this->m_enable = enbale;
}

bool Node::IsEnable() const
{
    return m_enable;
}

void Node::SetName(std::string name)
{
    this->m_name = name;
}

std::string Node::GetName() const
{
    return m_name;
}

bool Node::IsRoot() const
{
    if(m_sceneManager)
        return (this == m_sceneManager->GetRootNode());
    else
        return false;
}

bool Node::IsAttached() const
{
    return m_parent;
}

void Node::ReleaseParent()
{
    if(!m_parent)
        return;

    m_parent->ReleaseChild(this);
    m_parent = NULL;
}

unsigned Node::DeepPosition() const
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

void Node::SetParent(Node* parent)
{
    parent->AddChild(this);
}

Node* Node::GetParent() const
{
    return m_parent;
}

void Node::AddChild(Node* child)
{
    if(find(m_childs.begin(), m_childs.end(), child) != m_childs.end())
        throw Exception("Node::AddChild; child already exist");

    if(child->m_parent)
        child->m_parent->ReleaseChild(child);

    child->m_parent = this;
    child->m_sceneManager = m_sceneManager;

    m_childs.push_back(child);

    m_aabb.Count(child->GetAbsolutAabb());
}

void Node::ReleaseChild(Node* child)
{
    Node::Array::iterator it = find(m_childs.begin(), m_childs.end(), child);

    if(it == m_childs.end())
        throw Exception("Node::ReleaseChild; cannot found child");

    (*it)->m_parent = NULL;

    m_childs.erase(it);
}

Node* Node::ReleaseChild(unsigned index)
{
    if(index >= m_childs.size())
        throw Exception("Node::ReleaseChild; index out of range %d", index);

    Node* ret = m_childs[index];
    ret->ReleaseParent();

    m_childs.erase(m_childs.begin() + index);

    return ret;
}

void Node::DeleteChild(Node* child)
{
    Node::Array::iterator it = find(m_childs.begin(), m_childs.end(), child);

    if(it == m_childs.end())
        throw Exception("Node::DeleteChild; cannot found child");

    delete (*it);
    m_childs.erase(it);
}

void Node::DeleteChild(unsigned index)
{
    if(index >= m_childs.size())
        throw Exception("Node::DeleteChild; index out of range %d", index);

    delete m_childs[index];
    m_childs.erase(m_childs.begin() + index);
}

unsigned Node::GetChildCount() const
{
    return m_childs.size();
}

Node* Node::GetChild(unsigned index) const
{
    if(index >= m_childs.size())
        throw Exception("Node::GetChild; index out of range %d", index);

    return m_childs[index];
}

void Node::SetUserData(Any userData)
{
    this->m_userData = userData;
}

Any Node::GetUserData() const
{
    return m_userData;
}

Iterator<Node*> Node::GetChildIterator()
{
    return Iterator<Node*>(m_childs);
}

void Node::ClearAllChild()
{
    // Copie de pointeur pour éviter les probleme d'étiration
    // lors de la supprésion

    Node::Array dtor = m_childs;

    for(unsigned i = 0; i < dtor.size(); i++)
        delete dtor[i];
}

Node::CtorMap Node::ConstructionMap(std::string root)
{
    Node::CtorMap ctormap;

    ctormap["name"] = m_name;
    ctormap["matrix"] = Matrix4ToStr(m_matrix);

    return ctormap;
}