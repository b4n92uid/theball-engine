/*
 * File:   Skybox.cpp
 * Author: b4n92uid
 */

#include "Skybox.h"
#include "Material.h"
#include <algorithm>

using namespace std;
using namespace tbe;
using namespace tbe::scene;

SkyBox::SkyBox()
{
    m_enable = false;

    m_hardbuf = new HardwareBuffer;

    Vertex vertexs[24];
    // Devant
    vertexs[0] = Vertex(1.f, 1.f, 1.f, 0, 0, 0, 1, 1, 1, 1, 0, 0);
    vertexs[1] = Vertex(-1.f, 1.f, 1.f, 0, 0, 0, 1, 1, 1, 1, 1, 0);
    vertexs[2] = Vertex(-1.f, -1.f, 1.f, 0, 0, 0, 1, 1, 1, 1, 1, 1);
    vertexs[3] = Vertex(1.f, -1.f, 1.f, 0, 0, 0, 1, 1, 1, 1, 0, 1);
    // Derrière
    vertexs[4] = Vertex(-1.f, 1.f, -1.f, 0, 0, 0, 1, 1, 1, 1, 0, 0);
    vertexs[5] = Vertex(1.f, 1.f, -1.f, 0, 0, 0, 1, 1, 1, 1, 1, 0);
    vertexs[6] = Vertex(1.f, -1.f, -1.f, 0, 0, 0, 1, 1, 1, 1, 1, 1);
    vertexs[7] = Vertex(-1.f, -1.f, -1.f, 0, 0, 0, 1, 1, 1, 1, 0, 1);
    // Haut
    vertexs[8] = Vertex(-1.f, 1.f, -1.f, 0, 0, 0, 1, 1, 1, 1, 0, 0);
    vertexs[9] = Vertex(-1.f, 1.f, 1.f, 0, 0, 0, 1, 1, 1, 1, 1, 0);
    vertexs[10] = Vertex(1.f, 1.f, 1.f, 0, 0, 0, 1, 1, 1, 1, 1, 1);
    vertexs[11] = Vertex(1.f, 1.f, -1.f, 0, 0, 0, 1, 1, 1, 1, 0, 1);
    // Bas
    vertexs[12] = Vertex(-1.f, -1.f, -1.f, 0, 0, 0, 1, 1, 1, 1, 0, 1);
    vertexs[13] = Vertex(1.f, -1.f, -1.f, 0, 0, 0, 1, 1, 1, 1, 0, 0);
    vertexs[14] = Vertex(1.f, -1.f, 1.f, 0, 0, 0, 1, 1, 1, 1, 1, 0);
    vertexs[15] = Vertex(-1.f, -1.f, 1.f, 0, 0, 0, 1, 1, 1, 1, 1, 1);
    // Gauche
    vertexs[16] = Vertex(1.f, 1.f, -1.f, 0, 0, 0, 1, 1, 1, 1, 0, 0);
    vertexs[17] = Vertex(1.f, 1.f, 1.f, 0, 0, 0, 1, 1, 1, 1, 1, 0);
    vertexs[18] = Vertex(1.f, -1.f, 1.f, 0, 0, 0, 1, 1, 1, 1, 1, 1);
    vertexs[19] = Vertex(1.f, -1.f, -1.f, 0, 0, 0, 1, 1, 1, 1, 0, 1);
    // Droite
    vertexs[20] = Vertex(-1.f, 1.f, 1.f, 0, 0, 0, 1, 1, 1, 1, 0, 0);
    vertexs[21] = Vertex(-1.f, 1.f, -1.f, 0, 0, 0, 1, 1, 1, 1, 1, 0);
    vertexs[22] = Vertex(-1.f, -1.f, -1.f, 0, 0, 0, 1, 1, 1, 1, 1, 1);
    vertexs[23] = Vertex(-1.f, -1.f, 1.f, 0, 0, 0, 1, 1, 1, 1, 0, 1);

    for(unsigned i = 0; i < 24; i++)
        vertexs[i].pos *= 10;

    m_hardbuf->addVertex(vertexs, 24);
    m_hardbuf->compile();
}

SkyBox::SkyBox(Texture textures[])
{
    m_enable = false;
    setTextures(textures);
}

SkyBox::~SkyBox()
{
    clear();
    delete m_hardbuf;
}

Texture* SkyBox::getTextures()
{
    return m_textures;
}

void SkyBox::clear()
{
    for_each(m_textures, m_textures + 6, mem_fun_ref(&Texture::release));

    m_enable = false;
}

void SkyBox::setTextures(Texture textures[])
{
    for(unsigned i = 0; i < 6; i++)
    {
        if(!textures[i])
            continue;

        m_textures[i] = textures[i];

        m_textures[i].use(true);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        m_textures[i].use(false);
    }
}

void SkyBox::render(const Vector3f& camerapos)
{
    if(!m_textures || !m_enable)
        return;

    glPushMatrix();
    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_ALPHA);
    glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_CLIP_PLANE0);

    Shader::unbind();

    glTranslatef(camerapos.x, camerapos.y, camerapos.z);

    m_hardbuf->bindBuffer();
    m_hardbuf->bindTexture();

    for(int i = 0; i < 6; i++)
    {
        m_textures[i].use(true);
        m_hardbuf->render(Material::QUADS, i * 4, 4);
    }

    m_hardbuf->bindTexture(false);
    m_hardbuf->unbindBuffer();

    glPopAttrib();
    glPopMatrix();
}

void SkyBox::setEnable(bool enable)
{
    this->m_enable = enable;
}

bool SkyBox::isEnable() const
{
    return m_enable;
}
