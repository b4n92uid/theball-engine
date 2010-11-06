/* 
 * File:   HardwareBuffer.cpp
 * Author: b4n92uid
 * 
 * Created on 22 mai 2010, 11:56
 */

#include "HardwareBuffer.h"

#include <algorithm>
#include <iostream>

#define vertexStrid     sizeof(Vertex)

#define posOffset       (void*)(0)
#define normalOffset    (void*)(unsigned(posOffset)     + sizeof(Vector3f))
#define colorOffset     (void*)(unsigned(normalOffset)  + sizeof(Vector3f))
#define texUvOffset     (void*)(unsigned(colorOffset)   + sizeof(Vector4f))
#define tangentOffset   (void*)(unsigned(texUvOffset)   + sizeof(Vector2f))
#define aoccOffset      (void*)(unsigned(tangentOffset) + sizeof(Vector3f))

using namespace tbe;
using namespace std;

HardwareBuffer::HardwareBuffer()
{
    glGenBuffers(1, &m_bufferId);

    m_vertexCount = 0;
    m_bufferSize = 0;
}

HardwareBuffer::~HardwareBuffer()
{
    glDeleteBuffers(1, &m_bufferId);
}

HardwareBuffer::HardwareBuffer(const HardwareBuffer& hb)
{
    glGenBuffers(1, &m_bufferId);

    *this = hb;
}

bool HardwareBuffer::operator=(const HardwareBuffer& hb)
{
    m_vertex = hb.m_vertex;

    Compile(hb.m_usage);

    return true;
}

Vertex* HardwareBuffer::Lock(GLenum usage)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
    Vertex* offset = static_cast<Vertex*>(glMapBuffer(GL_ARRAY_BUFFER, usage));

    return offset;
}

void HardwareBuffer::UnLock()
{
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool HardwareBuffer::IsEmpty()
{
    return !m_vertexCount && !m_bufferSize;
}

void HardwareBuffer::AddFace(const Face& face)
{
    m_vertex.insert(m_vertex.end(), face.begin(), face.end());
}

void HardwareBuffer::AddVertex(const Vertex& vertex)
{
    m_vertex.push_back(vertex);
}

void HardwareBuffer::AddVertex(const Vertex* array, unsigned size)
{
    m_vertex.insert(m_vertex.end(), array, array + size);
}

void HardwareBuffer::AddVertex(const Vertex::Array& array)
{
    m_vertex.insert(m_vertex.end(), array.begin(), array.end());
}

void HardwareBuffer::Compile(GLenum usage)
{
    m_usage = usage;
    m_vertexCount = m_vertex.size();
    m_bufferSize = m_vertex.size() * sizeof(Vertex);

    if(m_vertex.size() % 3 > 0)
        cout << "HardwareBuffer::Compile; Mesh faces are not triangulated" << endl;

    glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
    glBufferData(GL_ARRAY_BUFFER, m_bufferSize, &m_vertex[0], usage);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void HardwareBuffer::BindBuffer(bool state)
{
    if(state)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, vertexStrid, posOffset);
    }

    else
    {
        glDisableClientState(GL_VERTEX_ARRAY);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void HardwareBuffer::BindNormal(bool state)
{
    if(state)
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, vertexStrid, normalOffset);
    }

    else
        glDisableClientState(GL_NORMAL_ARRAY);
}

void HardwareBuffer::BindColor(bool state)
{
    if(state)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, vertexStrid, colorOffset);
    }

    else
        glDisableClientState(GL_COLOR_ARRAY);
}

void HardwareBuffer::BindTexture(bool state)
{
    if(state)
    {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, vertexStrid, texUvOffset);
    }

    else
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void HardwareBuffer::BindTangent(bool state, GLint location)
{
    if(state)
    {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 3, GL_FLOAT, 0, vertexStrid, (void*)tangentOffset);
    }

    else
        glDisableVertexAttribArray(location);
}

void HardwareBuffer::BindAocc(bool state, GLint location)
{
    if(state)
    {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 4, GL_FLOAT, 0, vertexStrid, (void*)aoccOffset);
    }

    else
        glDisableVertexAttribArray(location);
}

void HardwareBuffer::Render(GLenum mode, unsigned first, unsigned count)
{
    glDrawArrays(mode, first, count ? count : m_vertexCount);
}

unsigned HardwareBuffer::GetBufferSize() const
{
    return m_bufferSize;
}

unsigned HardwareBuffer::GetVertexCount() const
{
    return m_vertexCount;
}

inline bool VertexComparePredicat(Vertex& v1, Vertex& v2)
{
    return(v1.pos == v2.pos);
}

Face::Array HardwareBuffer::GetAllFace()
{
    Face::Array faceArray;
    faceArray.reserve(m_vertexCount / 3);

    Vertex* vertex = Lock(GL_READ_ONLY);

    for(unsigned i = 0; i < m_vertexCount; i += 3)
    {
        Face f;
        f.push_back(vertex[i + 0]);
        f.push_back(vertex[i + 1]);
        f.push_back(vertex[i + 2]);

        faceArray.push_back(f);
    }

    UnLock();

    return faceArray;
}

Vertex::Array HardwareBuffer::GetAllVertex(bool makeUnique)
{
    Vertex::Array allVertexs;

    Vertex* vertex = Lock(GL_READ_ONLY);
    allVertexs.assign(vertex, vertex + m_vertexCount);
    UnLock();

    if(makeUnique)
    {
        Vertex::Array::iterator newEnd = unique(allVertexs.begin(), allVertexs.end(), VertexComparePredicat);
        allVertexs.erase(newEnd, allVertexs.end());
    }

    return allVertexs;
}

// Vertex ----------------------------------------------------------------------

Vertex::Vertex()
{

}

Vertex::Vertex(Vector3f pos, Vector3f normal, Vector4f color, Vector2f texCoord)
{
    this->pos = pos;
    this->normal = normal;
    this->color = color;
    this->texCoord = texCoord;
}

Vertex::Vertex(float posx, float posy, float posz,
               float normalx, float normaly, float normalz,
               float colorx, float colory, float colorz, float colorw,
               float texCoordx, float texCoordy)
{
    pos.x = posx;
    pos.y = posy;
    pos.z = posz;
    normal.x = normalx;
    normal.y = normaly;
    normal.z = normalz;
    color.x = colorx;
    color.y = colory;
    color.z = colorz;
    color.w = colorw;
    texCoord.x = texCoordx;
    texCoord.y = texCoordy;
}
