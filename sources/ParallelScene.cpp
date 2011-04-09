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

void ParallelScene::setSceneManager(SceneManager* sceneManager)
{
    this->m_sceneManager = sceneManager;
}

SceneManager* ParallelScene::getSceneManager() const
{
    return m_sceneManager;
}

void ParallelScene::setEnable(bool enable)
{
    this->m_enable = enable;
}

bool ParallelScene::isEnable() const
{
    return m_enable;
}
