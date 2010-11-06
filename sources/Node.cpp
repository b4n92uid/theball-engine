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
    m_matrixParent = NULL;
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

void Node::SetMatrixParent(Node* target)
{
    m_matrixParent = target;
}
