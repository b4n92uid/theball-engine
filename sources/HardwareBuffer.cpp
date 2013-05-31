/*
 * File:   HardwareBuffer.cpp
 * Author: b4n92uid
 *
 * Created on 22 mai 2010, 11:56
 */

#include "HardwareBuffer.h"
#include "Exception.h"
#include "Mathematics.h"
#include "Mesh.h"

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
    m_indexId = 0;

    glGenBuffersARB(1, &m_bufferId);
    glGenBuffersARB(1, &m_indexId);

    if(!m_bufferId)
        throw Exception("HardwareBuffer::HardwareBuffer; Buffer generation failed");

    m_vertexCount = 0;
    m_bufferSize = 0;
    m_multiTexCoordOffset = 0;

    m_indexCount = 0;
    m_indexBufferSize = 0;
    m_indexMode = false;

    m_cache = NULL;
}

HardwareBuffer::HardwareBuffer(const HardwareBuffer& hb)
{
    m_bufferId = 0;
    m_indexId = 0;

    glGenBuffersARB(1, &m_bufferId);
    glGenBuffersARB(1, &m_indexId);

    if(!m_bufferId)
        throw Exception("HardwareBuffer::HardwareBuffer; Buffer generation failed");

    m_vertexCount = 0;
    m_bufferSize = 0;
    m_multiTexCoordOffset = 0;

    m_indexCount = 0;
    m_indexBufferSize = 0;
    m_indexMode = false;

    *this = hb;

    m_cache = NULL;
}

HardwareBuffer::~HardwareBuffer()
{
    glDeleteBuffersARB(1, &m_bufferId);
    glDeleteBuffersARB(1, &m_indexId);
}

HardwareBuffer& HardwareBuffer::operator=(const HardwareBuffer& hb)
{
    m_vertex = hb.m_vertex;
    m_index = hb.m_index;
    m_multiTexCoord = hb.m_multiTexCoord;

    compile(hb.m_usage);

    if(hb.m_indexMode)
        compileIndex(hb.m_usage);

    return *this;
}

Vertex* HardwareBuffer::lock(GLenum usage)
{
    if(m_cache)
        return m_cache;
    else
        return m_cache = static_cast<Vertex*> (glMapBufferARB(GL_ARRAY_BUFFER_ARB, usage));
}

Vector2f* HardwareBuffer::lockMultiTexCoord(unsigned index, GLenum usage)
{
    if(index < 1)
        return NULL;

    long base = m_cache ? (long) m_cache : (long) glMapBufferARB(GL_ARRAY_BUFFER_ARB, usage);
    long offset = m_multiTexCoordOffset + (index - 1) * m_vertexCount * sizeof (Vector2f);

    return reinterpret_cast<Vector2f*> (base + offset);
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

void HardwareBuffer::addIndex(unsigned index)
{
    m_index.push_back(index);
}

void HardwareBuffer::addIndex(unsigned* array, unsigned size)
{
    m_index.insert(m_index.end(), array, array + size);
}

void HardwareBuffer::clear()
{
    m_vertex.clear();
    m_vertexCount = 0;
    m_bufferSize = 0;
}

void HardwareBuffer::convertToIndexedBuffer()
{
    Vertex::Array indexedVertices = m_vertex;

    m_vertex.clear();
    m_index.clear();

    for(unsigned i = 0; i < indexedVertices.size(); i++)
    {
        Vertex& v = indexedVertices[i];

        Vertex::Array::iterator it = std::find(m_vertex.begin(), m_vertex.end(), v);

        if(it != m_vertex.end())
        {
            unsigned index = it - m_vertex.begin();
            m_index.push_back(index);
        }
        else
        {
            m_vertex.push_back(v);
            m_index.push_back(m_vertex.size() - 1);
        }
    }
}

void HardwareBuffer::compile(GLenum usage)
{
    typedef std::map<unsigned, Vector2f::Array> vu2map;

    bool allocate = m_vertexCount == 0;

    m_usage = usage;
    m_vertexCount = m_vertex.size();
    m_bufferSize = m_vertex.size() * sizeof (Vertex);

    unsigned coreBufferSize = m_bufferSize;

    if(!m_multiTexCoord.empty())
    {
        m_multiTexCoordOffset = m_bufferSize;
        m_bufferSize += m_multiTexCoord.size() * sizeof (Vector2f);
    }

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_bufferId);

    if(allocate)
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_bufferSize, &m_vertex[0], usage);
    else
        glBufferSubData(GL_ARRAY_BUFFER_ARB, 0, coreBufferSize, &m_vertex[0]);

    unsigned mtoffset = m_multiTexCoordOffset;

    for(vu2map::iterator it = m_multiTexCoord.begin(); it != m_multiTexCoord.end(); it++)
    {
        int size = it->second.size() * sizeof (Vector2f);
        glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, mtoffset, size, &it->second[0]);

        mtoffset += size;
    }

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

    drawGlError();
}

void HardwareBuffer::compileIndex(GLenum usage)
{
    bool allocate = m_indexCount == 0;

    m_usage = usage;
    m_indexCount = m_index.size();
    m_indexBufferSize = m_index.size() * sizeof (unsigned);

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_indexId);

    if(allocate)
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_indexBufferSize, &m_index[0], usage);
    else
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER_ARB, 0, m_indexBufferSize, &m_index[0]);

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

    m_indexMode = true;

    drawGlError();
}

HardwareBuffer& HardwareBuffer::bindBuffer(bool state, int vertCount)
{
    if(state)
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_bufferId);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(vertCount, GL_FLOAT, vertexStrid, posOffset);
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

        if(layer == 0)
        {
            glTexCoordPointer(2, GL_FLOAT, vertexStrid, texUvOffset);
        }
        else
        {
            if(m_multiTexCoord.count(layer))
                glTexCoordPointer(2, GL_FLOAT, 0, (void*) (m_multiTexCoordOffset
                                  + (layer - 1) * m_vertexCount * sizeof (Vector2f)));

            else
                glTexCoordPointer(2, GL_FLOAT, vertexStrid, texUvOffset);
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
        glVertexAttribPointer(location, 3, GL_FLOAT, 0, vertexStrid, (void*) tangentOffset);
    }

    else
        glDisableVertexAttribArray(location);
}

void HardwareBuffer::bindAocc(bool state, GLint location)
{
    if(state)
    {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 4, GL_FLOAT, 0, vertexStrid, (void*) aoccOffset);
    }

    else
        glDisableVertexAttribArray(location);
}

void HardwareBuffer::render(GLenum mode, unsigned first, unsigned count, int drawpass)
{
    if(m_vertexCount == 0)
        return;

    if(m_indexMode)
    {
        // TODO Support of drawpass

        if(count == 0)
            count = m_indexCount;

        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_indexId);
        glDrawElements(mode, count, GL_UNSIGNED_INT, (void*) (first * sizeof (unsigned)));
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
        return;
    }

    if(count == 0)
        count = m_vertexCount;

    if(drawpass > 1)
    {
        int subcount = count / drawpass;
        int rest = count % drawpass;

        for(int i = 0; i < drawpass; i++)
            glDrawArrays(mode, first + i * subcount, subcount);

        if(rest > 0)
            glDrawArrays(mode, first + (drawpass - 1) * subcount, rest);
    }
    else
        glDrawArrays(mode, first, count);
}

unsigned HardwareBuffer::getBufferSize() const
{
    return m_bufferSize;
}

unsigned HardwareBuffer::getVertexCount() const
{
    return m_vertex.size();
}

bool HardwareBuffer::isIndexMode() const
{
    return m_indexMode;
}

unsigned HardwareBuffer::getIndexCount() const
{
    return m_indexCount;
}

inline bool VertexComparePredicat(Vertex& v1, Vertex& v2)
{
    return (v1.pos == v2.pos);
}

Face::Array HardwareBuffer::getAllFace()
{
    Face::Array faceArray;
    faceArray.reserve(m_vertexCount / 3);

    for(unsigned i = 0; i < m_vertexCount; i += 3)
    {
        Face f;
        f.push_back(m_vertex[i + 0]);
        f.push_back(m_vertex[i + 1]);
        f.push_back(m_vertex[i + 2]);

        faceArray.push_back(f);
    }

    return faceArray;
}

Vertex::Array HardwareBuffer::getAllVertex(bool makeUnique)
{
    Vertex::Array allVertexs = m_vertex;

    if(makeUnique)
    {
        Vertex::Array::iterator newEnd = unique(allVertexs.begin(), allVertexs.end(), VertexComparePredicat);
        allVertexs.erase(newEnd, allVertexs.end());
    }

    return allVertexs;
}

const Vertex::Array& HardwareBuffer::getClientVertex() const
{
    return m_vertex;
}

const HardwareBuffer::IndexArray& HardwareBuffer::getClientIndex() const
{
    return m_index;
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

Vertex::Vertex() { }

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

bool Vertex::operator==(const Vertex& v) const
{
    return math::isEqual(this->pos, v.pos) && math::isEqual(this->normal, v.normal)
            && math::isEqual(this->color, v.color) && math::isEqual(this->texCoord, v.texCoord);
}
