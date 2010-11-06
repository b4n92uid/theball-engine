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
}

Node::~Node()
{
}

bool Node::operator ==(std::string name)
{
    return m_name == name;
}

bool Node::operator ==(Node* node)
{
    return m_name == node->m_name;
}

void Node::Process()
{
}

void Node::Render()
{
}

void Node::SetPos(Vector3f pos)
{
    m_matrix.SetPos(pos);
}

Vector3f Node::GetPos() const
{
    return m_matrix.GetPos();
}

void Node::SetMatrix(const Matrix4f& matrix)
{
    this->m_matrix = matrix;
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

void Node::SetParent(Node* parent)
{
    if(m_parent)
        m_parent->ReleaseChild(this);

    parent->AddChild(this);
}

Node* Node::GetParent()
{
    return m_parent;
}

void Node::AddChild(Node* child)
{
    if(find(m_childs.begin(), m_childs.end(), child) == m_childs.end())
    {
        m_aabb += child->m_aabb;
        m_childs.push_back(child);
    }

    else
        throw Exception("Node::AddChild; child already exist");
}

Node* Node::ReleaseChild(Node* child)
{
    Node::Array::iterator it = find(m_childs.begin(), m_childs.end(), child);

    if(it == m_childs.end())
        throw Exception("Node::ReleaseChild; cannot found child");

    Node* ret = *it;
    m_childs.erase(it);

    return ret;
}

Node* Node::ReleaseChild(unsigned index)
{
    if(index >= m_childs.size())
        throw Exception("Node::ReleaseChild; index out of range %d", index);

    Node* ret = m_childs[index];
    m_childs.erase(m_childs.begin() + index);

    return ret;
}

Node* Node::GetChild(unsigned index)
{
    if(index >= m_childs.size())
        throw Exception("Node::GetChild; index out of range %d", index);

    return m_childs[index];
}
