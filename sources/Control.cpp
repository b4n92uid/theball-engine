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

void Control::DrawSurface(Vector2f pos, Vector2f size, Vector2f tcOffset, Vector2f tcLength)
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

void Control::DrawSurface(Vector2f pos[], Vector2f st[], unsigned draw)
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

void Control::Render()
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
        m_output->Use(true);

    ObjectRender();

    if(m_output)
        m_output->Use(false);
}

void Control::SetSkin(const GuiSkin&)
{

}

void Control::Move(Vector2f relPos)
{
    m_pos += relPos;
}

void Control::SetPos(Vector2f pos)
{
    #ifdef TBE_COMPILE_DEBUG
    if(pos.x < 0)
        cout << "Control::SetPos; Warning : x = " << pos.x << endl;

    if(pos.y < 0)
        cout << "Control::SetPos; Warning : y = " << pos.y << endl;
    #endif

    m_pos = pos;
}

Vector2f Control::GetPos() const
{
    return m_pos;
}

void Control::SetBackground(Texture background)
{
    this->m_background = background;
}

Texture Control::GetBackground() const
{
    return m_background;
}

void Control::SetActivate(bool activate)
{
    this->m_activate = activate;
}

bool Control::IsActivate() const
{
    return m_activate;
}

void Control::SetStretch(bool stretch)
{
    this->m_stretch = stretch;
}

bool Control::IsStretch() const
{
    return m_stretch;
}

void Control::SetLabel(std::string label)
{
    m_label = label;
}

std::string Control::GetLabel()
{
    return m_label;
}

void Control::SetSize(Vector2f size)
{
    #ifdef TBE_COMPILE_DEBUG
    if(size.x <= 0)
        cout << "Control::SetSize; Warning : size.x = " << size.x << endl;

    if(size.y <= 0)
        cout << "Control::SetSize; Warning : size.y = " << size.y << endl;
    #endif

    m_size = size;
}

Vector2f Control::GetSize() const
{
    return m_size;
}

void Control::SetEnable(bool enable)
{
    this->m_enable = enable;
}

bool Control::IsEnable() const
{
    return m_enable;
}

void Control::SetEnableAlpha(bool enableAlpha)
{
    this->m_enableAlpha = enableAlpha;
}

bool Control::IsEnableAlpha() const
{
    return m_enableAlpha;
}

void Control::SetEnableBackground(bool enableBackground)
{
    this->m_enableBackground = enableBackground;
}

bool Control::IsEnableBackground() const
{
    return m_enableBackground;
}

void Control::SetOpacity(float opacity)
{
    this->m_opacity = opacity;
}

float Control::GetOpacity() const
{
    return m_opacity;
}

void Control::SetPencil(Pencil pencil)
{
    this->m_pencil = pencil;
}

Pencil Control::GetPencil() const
{
    return m_pencil;
}

void Control::SetName(std::string name)
{
    this->m_name = name;
}

std::string Control::GetName() const
{
    return m_name;
}

void Control::SetOutput(Rtt* output)
{
    this->m_output = output;
}

Rtt* Control::GetOutput() const
{
    return m_output;
}
