#include "Skybox.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

SkyBox::SkyBox()
{
    m_enable = false;
    m_renderID = 0;
}

SkyBox::SkyBox(Texture textures[])
{
    m_enable = false;
    SetTextures(textures);
}

SkyBox::~SkyBox()
{
    if(m_renderID)
    {
        glDeleteLists(m_renderID, 1);
        m_renderID = 0;
    }
}

Texture* SkyBox::GetTextures()
{
    return m_textures;
}

void SkyBox::SetTextures(Texture textures[])
{
    for(unsigned i = 0; i < 6; i++)
    {
        m_textures[i] = textures[i];

        m_textures[i].Use(true);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        m_textures[i].Use(false);
    }

    InitRender();
}

void SkyBox::InitRender()
{
    Vector3f size = 10;

    m_renderID = glGenLists(1);

    glNewList(m_renderID, GL_COMPILE);

    m_textures[0].Use(true);

    // Devant
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0);
    glVertex3f(size.x, size.y, size.z);
    glTexCoord2d(1, 0);
    glVertex3f(-size.x, size.y, size.z);
    glTexCoord2d(1, 1);
    glVertex3f(-size.x, -size.y, size.z);
    glTexCoord2d(0, 1);
    glVertex3f(size.x, -size.y, size.z);
    glEnd();

    m_textures[1].Use(true);

    // Derriére
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0);
    glVertex3f(-size.x, size.y, -size.z);
    glTexCoord2d(1, 0);
    glVertex3f(size.x, size.y, -size.z);
    glTexCoord2d(1, 1);
    glVertex3f(size.x, -size.y, -size.z);
    glTexCoord2d(0, 1);
    glVertex3f(-size.x, -size.y, -size.z);
    glEnd();

    m_textures[2].Use(true);

    // Haut
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0);
    glVertex3f(-size.x, size.y, -size.z);
    glTexCoord2d(1, 0);
    glVertex3f(-size.x, size.y, size.z);
    glTexCoord2d(1, 1);
    glVertex3f(size.x, size.y, size.z);
    glTexCoord2d(0, 1);
    glVertex3f(size.x, size.y, -size.z);
    glEnd();

    m_textures[3].Use(true);

    // Bas
    glBegin(GL_QUADS);
    glTexCoord2d(0, 1);
    glVertex3f(-size.x, -size.y, -size.z);
    glTexCoord2d(0, 0);
    glVertex3f(size.x, -size.y, -size.z);
    glTexCoord2d(1, 0);
    glVertex3f(size.x, -size.y, size.z);
    glTexCoord2d(1, 1);
    glVertex3f(-size.x, -size.y, size.z);
    glEnd();

    m_textures[4].Use(true);

    // Gauche
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0);
    glVertex3f(size.x, size.y, -size.z);
    glTexCoord2d(1, 0);
    glVertex3f(size.x, size.y, size.z);
    glTexCoord2d(1, 1);
    glVertex3f(size.x, -size.y, size.z);
    glTexCoord2d(0, 1);
    glVertex3f(size.x, -size.y, -size.z);
    glEnd();

    m_textures[5].Use(true);

    // Droite
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0);
    glVertex3f(-size.x, size.y, size.z);
    glTexCoord2d(1, 0);
    glVertex3f(-size.x, size.y, -size.z);
    glTexCoord2d(1, 1);
    glVertex3f(-size.x, -size.y, -size.z);
    glTexCoord2d(0, 1);
    glVertex3f(-size.x, -size.y, size.z);
    glEnd();

    glEndList();
}

void SkyBox::Render(const Vector3f& camerapos)
{
    if(!m_textures || !m_enable)
        return;

    glPushMatrix();
    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_TEXTURE_2D);

    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_CLIP_PLANE0);

    glTranslatef(camerapos.x, camerapos.y, camerapos.z);

    glCallList(m_renderID);

    glPopAttrib();
    glPopMatrix();
}

void SkyBox::SetEnable(bool enable)
{
    this->m_enable = enable;
}

bool SkyBox::IsEnable() const
{
    return m_enable;
}
