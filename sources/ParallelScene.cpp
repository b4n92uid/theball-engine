/* 
 * File:   ParallelScene.cpp
 * Author: b4n92uid
 * 
 * Created on 23 novembre 2009, 15:43
 */

#include "ParallelScene.h"
#include "SceneManager.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

ParallelScene::ParallelScene()
{
    m_sceneManager = NULL;
    m_enable = true;
}

ParallelScene::~ParallelScene()
{
    Clear();
}

void ParallelScene::Clear()
{
    for(unsigned i = 0; i < m_nodes.size(); i++)
        if(!m_nodes[i]->HasParent())
            if(!m_nodes[i]->IsLockPtr())
                delete m_nodes[i], m_nodes[i] = NULL;

    m_nodes.clear();
}

void ParallelScene::SetSceneManager(SceneManager* sceneManager)
{
    this->m_sceneManager = sceneManager;
}

SceneManager* ParallelScene::GetSceneManager() const
{
    return m_sceneManager;
}

void ParallelScene::SetEnable(bool enable)
{
    this->m_enable = enable;
}

bool ParallelScene::IsEnable() const
{
    return m_enable;
}

bool ParallelScene::IsChild(Node* child)
{
    return(find(m_nodes.begin(), m_nodes.end(), child) != m_nodes.end());
}

void ParallelScene::AddChild(Node* child)
{
    if(find(m_nodes.begin(), m_nodes.end(), child) != m_nodes.end())
        throw Exception("ParallelScene::AddChild; child already exist");

    child->SetParallelScene(this);

    m_nodes.push_back(child);
}

void ParallelScene::DeleteChild(Node* child)
{
    Node::Array::iterator it = find(m_nodes.begin(), m_nodes.end(), child);

    if(it == m_nodes.end())
        throw Exception("ParallelScene::DeleteChild; cannot found child");

    delete (*it);

    m_nodes.erase(it);
}

void ParallelScene::ReleaseChild(Node* child)
{
    Node::Array::iterator it = find(m_nodes.begin(), m_nodes.end(), child);

    if(it == m_nodes.end())
        throw Exception("ParallelScene::ReleaseChild; cannot found child");

    (*it)->SetParallelScene(NULL);

    m_nodes.erase(it);
}

Node* ParallelScene::ReleaseChild(unsigned index)
{
    if(index >= m_nodes.size())
        throw Exception("ParallelScene::ReleaseChild; index out of range %d", index);

    Node* child = m_nodes[index];
    child->SetParallelScene(NULL);

    m_nodes.erase(m_nodes.begin() + index);

    return child;
}

Node* ParallelScene::GetChild(unsigned index)
{
    if(index >= m_nodes.size())
        throw Exception("ParallelScene::GetChild; index out of range %d", index);

    return m_nodes[index];
}
