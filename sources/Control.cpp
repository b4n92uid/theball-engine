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
}

Control::~Control()
{
}

void Control::drawSurface(Vector2f pos, Vector2f size, Vector2f tcOffset, Vector2f tcLength)
{
    Vector2f vertexs[4] = {
        pos,
        Vector2f(pos.x + size.x, pos.y),
        Vector2f(pos.x, pos.y + size.y),
        pos + size,
    };

    Vector2f vTC[4] = {
        Vector2f(tcOffset.x, tcLength.y),
        Vector2f(tcLength.x, tcLength.y),
        Vector2f(tcOffset.x, tcOffset.y),
        Vector2f(tcLength.x, tcOffset.y),
    };

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, vertexs);
    glTexCoordPointer(2, GL_FLOAT, 0, vTC);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

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
