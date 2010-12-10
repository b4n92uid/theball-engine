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
