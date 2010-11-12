#include "Node.h"
#include "ParallelScene.h"

using namespace tbe;
using namespace tbe::scene;
using namespace std;

// Node ------------------------------------------------------------------------

Node::Node()
{
    m_enable = true;
    m_enableRender = true;
    m_enableProcess = true;
    m_lockPtr = false;
    m_parallelScene = NULL;

    m_parent = NULL;
}

Node::Node(const Node& copy)
{
    m_parent = NULL;

    *this = copy;
}

Node::~Node()
{
    for(unsigned i = 0; i < m_childs.size(); i++)
    {
        m_childs[i]->ReleaseFromParallelScene();

        if(!m_childs[i]->m_lockPtr)
            delete m_childs[i];
    }
}

Node& Node::operator =(const Node& copy)
{
    m_parallelScene = copy.m_parallelScene;
    m_name = copy.m_name;
    m_matrix = copy.m_matrix;
    m_enable = copy.m_enable;
    m_lockPtr = copy.m_lockPtr;
    m_aabb = copy.m_aabb;

    return *this;
}

bool Node::operator==(std::string name)
{
    return m_name == name;
}

bool Node::operator==(Node* node)
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

void Node::MulMatrix(const Matrix4f& matrix)
{
    this->m_matrix *= matrix;
}

void Node::SetMatrix(const Matrix4f& matrix)
{
    this->m_matrix = matrix;
}

Matrix4f Node::GetAbsoluteMatrix()
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

void Node::ReleaseFromParallelScene()
{
    if(!m_parallelScene)
        return;

    m_parallelScene->ReleaseChild(this);
    m_parallelScene = NULL;
}

AABB Node::GetAbsolutAabb() const
{
    Vector3f pos = m_matrix.GetPos();
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

bool Node::HasParent()
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

void Node::SetParent(Node* parent)
{
    parent->AddChild(this);
}

Node* Node::GetParent()
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

    m_childs.push_back(child);

    m_aabb += child->m_aabb;
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

Node* Node::GetChild(unsigned index)
{
    if(index >= m_childs.size())
        throw Exception("Node::GetChild; index out of range %d", index);

    return m_childs[index];
}

void Node::SetEnableProcess(bool enableProcess)
{
    this->m_enableProcess = enableProcess;
}

bool Node::IsEnableProcess() const
{
    return m_enableProcess;
}

void Node::SetEnableRender(bool enableRender)
{
    this->m_enableRender = enableRender;
}

bool Node::IsEnableRender() const
{
    return m_enableRender;
}

void Node::SetLockPtr(bool lockPtr)
{
    this->m_lockPtr = lockPtr;
}

bool Node::IsLockPtr() const
{
    return m_lockPtr;
}
