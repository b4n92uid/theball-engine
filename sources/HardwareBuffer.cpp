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
    m_multiTexCoordOffset = 0;

    m_cache = NULL;
}

HardwareBuffer::HardwareBuffer(const HardwareBuffer& hb)
{
    m_bufferId = 0;

    glGenBuffersARB(1, &m_bufferId);

    if(!m_bufferId)
        throw Exception("HardwareBuffer::HardwareBuffer; Buffer generation failed");

    m_vertexCount = 0;
    m_bufferSize = 0;
    m_multiTexCoordOffset = 0;

    *this = hb;

    m_cache = NULL;
}

HardwareBuffer::~HardwareBuffer()
{
    glDeleteBuffersARB(1, &m_bufferId);
}

HardwareBuffer& HardwareBuffer::operator=(const HardwareBuffer& hb)
{
    m_vertex = hb.m_vertex;
    m_multiTexCoord = hb.m_multiTexCoord;

    compile(hb.m_usage);

    return *this;
}

Vertex* HardwareBuffer::lock(GLenum usage)
{
    if(m_cache)
        return m_cache;
    else
        return m_cache = static_cast<Vertex*>(glMapBufferARB(GL_ARRAY_BUFFER_ARB, usage));
}

Vector2f* HardwareBuffer::lockMultiTexCoord(unsigned index, GLenum usage)
{
    if(index < 1)
        return NULL;

    long base = m_cache ? (long)m_cache : (long)glMapBufferARB(GL_ARRAY_BUFFER_ARB, usage);
    long offset = m_multiTexCoordOffset + (index - 1) * m_vertexCount * sizeof(Vector2f);

    return reinterpret_cast<Vector2f*>(base + offset);
}

HardwareBuffer& HardwareBuffer::unlock()
{
    m_cache = NULL;
    glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
    return *this;
}

bool HardwareBuffer::isEmpty() const
{
    return !m_vertex.size();
}

void HardwareBuffer::addFace(const Face& face)
{
    m_vertex.insert(m_vertex.end(), face.begin(), face.end());
}

void HardwareBuffer::addVertex(const Vertex& vertex)
{
    m_vertex.push_back(vertex);
}

void HardwareBuffer::addVertex(const Vertex* array, unsigned size)
{
    m_vertex.insert(m_vertex.end(), array, array + size);
}

void HardwareBuffer::addVertex(const Vertex::Array& array)
{
    m_vertex.insert(m_vertex.end(), array.begin(), array.end());
}

void HardwareBuffer::clear()
{
    m_vertex.clear();
    m_vertexCount = 0;
    m_bufferSize = 0;

    glDeleteBuffersARB(1, &m_bufferId);
    glGenBuffersARB(1, &m_bufferId);
}

void HardwareBuffer::compile(GLenum usage)
{
    typedef std::map<unsigned, Vector2f::Array> vu2map;

    m_usage = usage;
    m_vertexCount = m_vertex.size();
    m_bufferSize = m_vertex.size() * sizeof(Vertex);

    if(!m_multiTexCoord.empty())
    {
        m_bufferSize += m_vertex.size() * sizeof(Vector2f) * m_multiTexCoord.size();

        m_multiTexCoordOffset = m_vertex.size() * sizeof(Vertex);
    }

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_bufferId);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_bufferSize, &m_vertex[0], usage);

    unsigned mtoffset = m_multiTexCoordOffset;

    for(vu2map::iterator it = m_multiTexCoord.begin(); it != m_multiTexCoord.end(); it++)
    {
        int size = it->second.size() * sizeof(Vector2f);
        glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, mtoffset, size, &it->second[0]);

        mtoffset += size;
    }

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

HardwareBuffer& HardwareBuffer::bindBuffer(bool state)
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

    return *this;
}

HardwareBuffer& HardwareBuffer::unbindBuffer()
{
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

    return *this;
}

void HardwareBuffer::bindNormal(bool state)
{
    if(state)
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, vertexStrid, normalOffset);
    }

    else
        glDisableClientState(GL_NORMAL_ARRAY);
}

void HardwareBuffer::bindColor(bool state)
{
    if(state)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, vertexStrid, colorOffset);
    }

    else
        glDisableClientState(GL_COLOR_ARRAY);
}

void HardwareBuffer::bindTexture(bool state, unsigned layer)
{
    if(state)
    {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        if(!layer)
            glTexCoordPointer(2, GL_FLOAT, vertexStrid, texUvOffset);
        else
        {
            if(!m_multiTexCoord.count(layer - 1))
                glTexCoordPointer(2, GL_FLOAT, vertexStrid, texUvOffset);

            else
                glTexCoordPointer(2, GL_FLOAT, 0, (void*)(m_multiTexCoordOffset
                                  + (layer - 1) * m_vertexCount * sizeof(Vector2f)));
        }
    }

    else
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void HardwareBuffer::bindTangent(bool state, GLint location)
{
    if(state)
    {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 3, GL_FLOAT, 0, vertexStrid, (void*)tangentOffset);
    }

    else
        glDisableVertexAttribArray(location);
}

void HardwareBuffer::bindAocc(bool state, GLint location)
{
    if(state)
    {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 4, GL_FLOAT, 0, vertexStrid, (void*)aoccOffset);
    }

    else
        glDisableVertexAttribArray(location);
}

void HardwareBuffer::render(GLenum mode, unsigned first, unsigned count)
{
    if(m_vertexCount > 0)
        glDrawArrays(mode, first, count ? count : m_vertexCount);
}

unsigned HardwareBuffer::getBufferSize() const
{
    return m_bufferSize;
}

unsigned HardwareBuffer::getVertexCount() const
{
    return m_vertex.size();
}

inline bool VertexComparePredicat(Vertex& v1, Vertex& v2)
{
    return(v1.pos == v2.pos);
}

Face::Array HardwareBuffer::getAllFace()
{
    Face::Array faceArray;
    faceArray.reserve(m_vertexCount / 3);

    Vertex* vertex = bindBuffer().lock(GL_READ_ONLY_ARB);

    for(unsigned i = 0; i < m_vertexCount; i += 3)
    {
        Face f;
        f.push_back(vertex[i + 0]);
        f.push_back(vertex[i + 1]);
        f.push_back(vertex[i + 2]);

        faceArray.push_back(f);
    }

    unlock().bindBuffer(false);

    return faceArray;
}

Vertex::Array HardwareBuffer::getAllVertex(bool makeUnique)
{
    Vertex::Array allVertexs;

    Vertex* vertex = bindBuffer(true).lock(GL_READ_ONLY_ARB);
    allVertexs.assign(vertex, vertex + m_vertexCount);
    unlock().bindBuffer(false);

    if(makeUnique)
    {
        Vertex::Array::iterator newEnd = unique(allVertexs.begin(), allVertexs.end(), VertexComparePredicat);
        allVertexs.erase(newEnd, allVertexs.end());
    }

    return allVertexs;
}

const Vertex::Array& HardwareBuffer::getInitialVertex() const
{
    return m_vertex;
}

void HardwareBuffer::snapshot()
{
    Vertex* vs = lock(GL_READ_ONLY);
    std::copy(vs, vs + m_vertexCount, m_vertex.begin());
    unlock();
}

void HardwareBuffer::restore()
{
    Vertex* vs = lock(GL_WRITE_ONLY);
    std::copy(m_vertex.begin(), m_vertex.end(), vs);
    unlock();
}

void HardwareBuffer::newMultiTexCoord(unsigned index)
{
    m_multiTexCoord[index].clear();

    for(unsigned i = 0; i < m_vertex.size(); i++)
        m_multiTexCoord[index].push_back(m_vertex[i].texCoord);
}

void HardwareBuffer::setMultiTexCoord(unsigned index, Vector2f::Array uv)
{
    m_multiTexCoord[index] = uv;
}

bool HardwareBuffer::checkHardware()
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
