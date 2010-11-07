#include "Node.h"
#include "ParallelScene.h"

using namespace tbe;
using namespace tbe::scene;
using namespace std;

// Node ------------------------------------------------------------------------

Node::Node()
{
    m_enable = true;
    m_enableProcess = true;
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
}

bool Node::operator =(const Node& copy)
{
    m_parallelScene = copy.m_parallelScene;
    m_name = copy.m_name;
    m_matrix = copy.m_matrix;
    m_enable = copy.m_enable;
    m_enableProcess = copy.m_enableProcess;
    m_aabb = copy.m_aabb;

    // m_parent = copy.m_parent;

    for(unsigned i = 0; i < copy.m_childs.size(); i++)
        m_childs.push_back(copy.m_childs[i]->Clone());

    return true;
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

void Node::SetEnableProcess(bool enableProcess)
{
    this->m_enableProcess = enableProcess;
}

bool Node::IsEnableProcess() const
{
    return m_enableProcess;
}

void Node::SetName(std::string name)
{
    this->m_name = name;
}

std::string Node::GetName() const
{
    return m_name;
}

bool Node::IsTopLevel()
{
    return !m_parent;
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

    (*it)->SetParent(NULL);

    m_childs.erase(it);
}

Node* Node::ReleaseChild(unsigned index)
{
    if(index >= m_childs.size())
        throw Exception("Node::ReleaseChild; index out of range %d", index);

    Node* ret = m_childs[index];
    ret->SetParent(NULL);

    m_childs.erase(m_childs.begin() + index);

    return ret;
}

Node* Node::GetChild(unsigned index)
{
    if(index >= m_childs.size())
        throw Exception("Node::GetChild; index out of range %d", index);

    return m_childs[index];
}
