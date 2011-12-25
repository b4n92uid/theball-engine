/* 
 * File:   LightParallelScene.cpp
 * Author: b4n92uid
 * 
 * Created on 17 décembre 2010, 16:54
 */

#include "LightParallelScene.h"
#include "Light.h"

using namespace tbe;
using namespace scene;

LightParallelScene::LightParallelScene()
{
    glGetIntegerv(GL_MAX_LIGHTS, & m_maxlight);
}

LightParallelScene::~LightParallelScene()
{
    for(int i = 0; i < m_maxlight; i++)
        glDisable(GL_LIGHT0 + i);
}

void LightParallelScene::render()
{
    for(int i = 0; i < m_maxlight; i++)
        glDisable(GL_LIGHT0 + i);

    for(unsigned i = 0; i < m_nodes.size(); i++)
        if(m_nodes[i]->isAttached())
            m_nodes[i]->render();
}

int LightParallelScene::getNextLightID()
{
    int lid = -1;

    for(int id = 0; id < m_maxlight; id++)
    {
        if(!glIsEnabled(GL_LIGHT0 + id))
        {
            lid = GL_LIGHT0 + id;
            break;
        }

    }

    return lid;
}
