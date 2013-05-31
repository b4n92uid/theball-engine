/* 
 * File:   WaterParallelScene.cpp
 * Author: b4n92uid
 * 
 * Created on 21 mai 2010, 22:31
 */

#include "WaterParallelScene.h"

#include <boost/foreach.hpp>

#include "Water.h"
#include "Exception.h"
#include "Tools.h"
#include "SceneManager.h"
#include "Skybox.h"
#include "ShadowMap.h"

using namespace tbe;
using namespace scene;

WaterParallelScene::WaterParallelScene() { }

WaterParallelScene::~WaterParallelScene() { }

void WaterParallelScene::addOffscreenScene(ParallelScene* scene)
{
    m_offscreenScene.push_back(scene);
}

void WaterParallelScene::render()
{
    if(!m_enable)
        return;

    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    SkyBox* sky = m_sceneManager->getSkybox();
    Camera* cam = m_sceneManager->getCurCamera();

    // Disable some useless effects here !
    ShadowMap::enable = VolumetricLight::enable = false;

    for(unsigned i = 0; i < m_nodes.size(); i++)
    {
        Water* water = m_nodes[i];

        if(!water->isAttached())
            return;

        // Render Reflection
        water->beginReflection();
        sky->render(cam->getPos());
        BOOST_FOREACH(ParallelScene* scene, m_offscreenScene) scene->render();
        water->endReflection();

        // Render Refraction
        water->beginRefraction();
        sky->render(cam->getPos());
        BOOST_FOREACH(ParallelScene* scene, m_offscreenScene) scene->render();
        water->endRefraction();

        water->render();
    }

    ShadowMap::enable = VolumetricLight::enable = true;

    glPopAttrib();
}
