/* 
 * File:   ParticlesParallelScene.cpp
 * Author: b4n92uid
 * 
 * Created on 30 avril 2010, 23:09
 */

#include "ParticlesParallelScene.h"
#include "SceneManager.h"
#include "Particles.h"

#include "Tools.h"

#define POINT_SIZE_CURRENT 256
#define POINT_SIZE 128

using namespace tbe;
using namespace tbe::scene;

ParticlesParallelScene::ParticlesParallelScene()
{
    m_pointSprite = ParticlesEmiter::checkHardware();

    m_particleMinSize = 1.0;
    m_particleMaxSize = 64.0;

    if(m_pointSprite)
    {
        // Activer le remplacement du poit par la texture
        glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);

        // Taille Maximal supporte par la Carte Graphique
        glGetFloatv(GL_POINT_SIZE_MAX_ARB, &m_particleMaxSize);

        // Limit Max/Min taille
        glPointParameterfARB(GL_POINT_SIZE_MIN_ARB, m_particleMinSize);
        glPointParameterfARB(GL_POINT_SIZE_MAX_ARB, m_particleMaxSize);

        // Fondue depuis le centre
        glPointParameterfARB(GL_POINT_FADE_THRESHOLD_SIZE_ARB, 60.0f);
    }
}

ParticlesParallelScene::~ParticlesParallelScene()
{
}

void ParticlesParallelScene::render()
{
    if(!m_enable)
        return;

    glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BITS | GL_POINT_BIT);

    if(m_pointSprite)
        glEnable(GL_POINT_SPRITE_ARB); // Active le remplacement du point par la texture

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_CULL_FACE);

    if(m_pointSprite)
    {
        // Configuration de la taille des points suivant la formule :
        // derived_size = size * sqrtf (1 / (a + b * d + c * d), d : distance en Z par rapport � l'�cran.

        float height = m_sceneManager->getViewport().y;
        float fovy = m_sceneManager->getFovy();

        float pixelPerUnit = height / (2.0f * tan(fovy * 0.5f));
        float sqrtC = POINT_SIZE_CURRENT / (POINT_SIZE * pixelPerUnit);

        Vector3f distAtt(0, 0, sqrtC * sqrtC);

        glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION_ARB, distAtt);
    }

    for(unsigned i = 0; i < m_nodes.size(); i++)
        if(m_nodes[i]->isAttached())
            m_nodes[i]->render();

    glPopAttrib();
}

void ParticlesParallelScene::hardwareParticleMaxSize()
{
    glGetFloatv(GL_POINT_SIZE_MAX_ARB, &m_particleMaxSize);
}

void ParticlesParallelScene::setParticleMaxSize(float particleMaxSize)
{
    this->m_particleMaxSize = particleMaxSize;

    if(m_pointSprite)
        glPointParameterfARB(GL_POINT_SIZE_MAX_ARB, m_particleMaxSize);
}

float ParticlesParallelScene::getParticleMaxSize() const
{
    return m_particleMaxSize;
}

void ParticlesParallelScene::setParticleMinSize(float particleMinSize)
{
    this->m_particleMinSize = particleMinSize;

    if(m_pointSprite)
        glPointParameterfARB(GL_POINT_SIZE_MIN_ARB, m_particleMinSize);
}

float ParticlesParallelScene::getParticleMinSize() const
{
    return m_particleMinSize;
}
