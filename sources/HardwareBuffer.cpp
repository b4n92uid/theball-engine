/* 
 * File:   HardwareBuffer.cpp
 * Author: b4n92uid
 * 
 * Created on 22 mai 2010, 11:56
 */

#include "HardwareBuffer.h"
#include "Exception.h"

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
    m_bufferId = 0;

    glGenBuffersARB(1, &m_bufferId);

    if(!m_bufferId)
        throw Exception("HardwareBuffer::HardwareBuffer; Buffer generation failed");

    m_vertexCount = 0;
    m_bufferSize = 0;
}

HardwareBuffer::HardwareBuffer(const HardwareBuffer& hb)
{
    m_bufferId = 0;

    glGenBuffersARB(1, &m_bufferId);

    if(!m_bufferId)
        throw Exception("HardwareBuffer::HardwareBuffer; Buffer generation failed");

    *this = hb;
}

HardwareBuffer::~HardwareBuffer()
{
    glDeleteBuffersARB(1, &m_bufferId);
}

HardwareBuffer& HardwareBuffer::operator=(const HardwareBuffer& hb)
{
    m_vertex = hb.m_vertex;

    Compile(hb.m_usage);

    return *this;
}

Vertex* HardwareBuffer::Lock(GLenum usage)
{
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_bufferId);
    Vertex* offset = static_cast<Vertex*>(glMapBufferARB(GL_ARRAY_BUFFER_ARB, usage));

    return offset;
}

void HardwareBuffer::UnLock()
{
    glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
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
    m_bufferSize = m_vertex.size() * sizeof (Vertex);

    if(m_vertex.size() % 3 > 0)
        cout << "HardwareBuffer::Compile; Mesh faces are not triangulated" << endl;

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_bufferId);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_bufferSize, &m_vertex[0], usage);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

void HardwareBuffer::BindBuffer(bool state)
{
    if(state)
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_bufferId);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, vertexStrid, posOffset);
    }

    else
    {
        glDisableClientState(GL_VERTEX_ARRAY);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
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
    return m_vertex.size();
}

inline bool VertexComparePredicat(Vertex& v1, Vertex& v2)
{
    return (v1.pos == v2.pos);
}

Face::Array HardwareBuffer::GetAllFace()
{
    Face::Array faceArray;
    faceArray.reserve(m_vertexCount / 3);

    Vertex* vertex = Lock(GL_READ_ONLY_ARB);

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

    Vertex* vertex = Lock(GL_READ_ONLY_ARB);
    allVertexs.assign(vertex, vertex + m_vertexCount);
    UnLock();

    if(makeUnique)
    {
        Vertex::Array::iterator newEnd = unique(allVertexs.begin(), allVertexs.end(), VertexComparePredicat);
        allVertexs.erase(newEnd, allVertexs.end());
    }

    return allVertexs;
}

bool HardwareBuffer::CheckHardware()
{
    return GLEE_ARB_vertex_buffer_object;
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
