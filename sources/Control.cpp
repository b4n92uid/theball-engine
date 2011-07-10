#include "Control.h"
#include "Rtt.h"

using namespace std;
using namespace tbe;
using namespace tbe::gui;

Control::Control()
{
    m_enable = true;
    m_enableAlpha = true;
    m_enableBackground = true;
    m_activate = false;
    m_stretch = false;
    m_opacity = 1.0;
    m_output = NULL;
    m_metaCount = 3;
    m_textAlign = HCENTER | VCENTER;
}

Control::~Control()
{
}

tbe::Vector2f::Array Control::vertexPos(tbe::Vector2f pos, tbe::Vector2f size)
{
    Vector2f vPos[6] = {
        pos,
        Vector2f(pos.x + size.x, pos.y),
        pos + size,
        pos,
        pos + size,
        Vector2f(pos.x, pos.y + size.y),
    };

    return Vector2f::Array(vPos, vPos + 6);
}

tbe::Vector2f::Array Control::vertexUv(tbe::Vector2f offset, tbe::Vector2f length)
{
    Vector2f vUv[6] = {
        Vector2f(offset.x, length.y), // 0,1
        Vector2f(length.x, length.y), // 1,1
        Vector2f(length.x, offset.y), // 1,0
        Vector2f(offset.x, length.y), // 0,1
        Vector2f(length.x, offset.y), // 1,0
        Vector2f(offset.x, offset.y), // 0,0
    };

    return Vector2f::Array(vUv, vUv + 6);
}

void Control::drawBackground()
{
    if(m_enableBackground && m_background)
    {
        if(m_backgroundMask)
        {
            Vector2f::Array vPos = vertexPos(m_pos, m_size);
            Vector2f::Array vUvR = vertexUv(0, m_size / m_background.getSize());
            Vector2f::Array vUv1 = vertexUv(0, 1);

            glEnableClientState(GL_VERTEX_ARRAY);

            glVertexPointer(2, GL_FLOAT, 0, &vPos[0]);

            // Texture 0 : VAO
            glClientActiveTexture(GL_TEXTURE0);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, 0, &vUvR[0]);

            // Texture 0
            glActiveTexture(GL_TEXTURE0);
            glEnable(GL_TEXTURE_2D);

            m_background.use(true);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

            // Texture 1 : VAO
            glClientActiveTexture(GL_TEXTURE1);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, 0, &vUv1[0]);

            // Texture 1
            glActiveTexture(GL_TEXTURE1);
            glEnable(GL_TEXTURE_2D);

            m_backgroundMask.use(true);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Disable Texture 1 : VAO
            glClientActiveTexture(GL_TEXTURE1);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);

            // Disable Texture 1
            glActiveTexture(GL_TEXTURE1);
            glDisable(GL_TEXTURE_2D);

            // Disable Texture 0 : VAO
            glClientActiveTexture(GL_TEXTURE0);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);

            // Disable Texture 0
            glActiveTexture(GL_TEXTURE0);

            m_background.use(false);

            glDisableClientState(GL_VERTEX_ARRAY);
        }

        else
        {
            m_background.use(true);
            drawSurface(m_pos, m_size, 0, m_size / m_background.getSize());
            m_background.use(false);
        }
    }
}

void Control::drawSurface(Vector2f pos, Vector2f size, Vector2f tcOffset, Vector2f tcLength)
{
    Vector2f::Array vPos = vertexPos(pos, size);
    Vector2f::Array vUv = vertexUv(tcOffset, tcLength);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, &vPos[0]);
    glTexCoordPointer(2, GL_FLOAT, 0, &vUv[0]);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Control::drawSurface(Vector2f pos[], Vector2f st[], unsigned draw)
{
    vector<Vector2f> _pos(pos, pos + draw);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    for(unsigned i = 0; i < draw; i++)
    {
        _pos[i] *= m_size;
        _pos[i] += m_pos;
    }

    glVertexPointer(2, GL_FLOAT, 0, &_pos[0]);
    glTexCoordPointer(2, GL_FLOAT, 0, st);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, draw);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Control::render()
{
    if(!m_enable)
        return;

    if(m_enableAlpha)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(1, 1, 1, m_opacity);
    }

    else
    {
        glDisable(GL_BLEND);

        glColor4f(1, 1, 1, 1);
    }

    if(m_output)
        m_output->use(true);

    objectRender();

    if(m_output)
        m_output->use(false);
}

void Control::setSkin(const GuiSkin&)
{

}

void Control::move(Vector2f relPos)
{
    m_pos += relPos;
}

void Control::setPos(Vector2f pos)
{
    #ifdef TBE_COMPILE_DEBUG
    if(pos.x < 0)
        cout << "Control::SetPos; Warning : x = " << pos.x << endl;

    if(pos.y < 0)
        cout << "Control::SetPos; Warning : y = " << pos.y << endl;
    #endif

    m_pos = pos;
}

Vector2f Control::getPos() const
{
    return m_pos;
}

void Control::setBackground(Texture background)
{
    this->m_background = background;
}

Texture Control::getBackground() const
{
    return m_background;
}

void Control::setBackgroundMask(Texture backgroundMask)
{
    this->m_backgroundMask = backgroundMask;
}

Texture Control::getBackgroundMask() const
{
    return m_backgroundMask;
}

void Control::setActivate(bool activate)
{
    this->m_activate = activate;
}

bool Control::isActivate() const
{
    return m_activate;
}

void Control::setStretch(bool stretch)
{
    this->m_stretch = stretch;
}

bool Control::isStretch() const
{
    return m_stretch;
}

void Control::setLabel(std::string label)
{
    m_label = label;
}

std::string Control::getLabel()
{
    return m_label;
}

void Control::setSize(Vector2f size)
{
    #ifdef TBE_COMPILE_DEBUG
    if(size.x <= 0)
        cout << "Control::SetSize; Warning : size.x = " << size.x << endl;

    if(size.y <= 0)
        cout << "Control::SetSize; Warning : size.y = " << size.y << endl;
    #endif

    m_size = size;
}

Vector2f Control::getSize() const
{
    return m_size;
}

void Control::setEnable(bool enable)
{
    this->m_enable = enable;
}

bool Control::isEnable() const
{
    return m_enable;
}

void Control::setEnableAlpha(bool enableAlpha)
{
    this->m_enableAlpha = enableAlpha;
}

bool Control::isEnableAlpha() const
{
    return m_enableAlpha;
}

void Control::setEnableBackground(bool enableBackground)
{
    this->m_enableBackground = enableBackground;
}

bool Control::isEnableBackground() const
{
    return m_enableBackground;
}

void Control::setOpacity(float opacity)
{
    this->m_opacity = opacity;
}

float Control::getOpacity() const
{
    return m_opacity;
}

void Control::setPencil(Pencil pencil)
{
    this->m_pencil = pencil;
}

Pencil Control::getPencil() const
{
    return m_pencil;
}

void Control::setName(std::string name)
{
    this->m_name = name;
}

std::string Control::getName() const
{
    return m_name;
}

void Control::setOutput(Rtt* output)
{
    this->m_output = output;
}

Rtt* Control::getOutput() const
{
    return m_output;
}

void Control::setMetaCount(int metaCount)
{
    this->m_metaCount = metaCount;
}

int Control::getMetaCount() const
{
    return m_metaCount;
}

void Control::setTextAlign(unsigned textAlign)
{
    this->m_textAlign = textAlign;
}

unsigned Control::getTextAlign() const
{
    return m_textAlign;
}

void Control::setTextPadding(Dimension textPadding)
{
    this->m_textPadding = textPadding;
}

Dimension Control::getTextPadding() const
{
    return m_textPadding;
}
