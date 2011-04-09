/* 
 * File:   HardwareBuffer.h
 * Author: b4n92uid
 *
 * Created on 22 mai 2010, 11:56
 */

#ifndef _HARDWAREBUFFER_H
#define	_HARDWAREBUFFER_H

#include <vector>

#include "GLee.h"

#include "Mathematics.h"
#include "AABB.h"

namespace tbe
{

/// \brief Représentation d'un vertex

class Vertex
{
public:
    Vertex();
    Vertex(Vector3f pos, Vector3f normal, Vector4f color, Vector2f texCoord);
    Vertex(float px, float py, float pz, float nx, float ny, float nz, float r, float g, float b, float a, float s, float t);

    typedef std::vector<Vertex> Array;

public:
    Vector3f pos;
    Vector3f normal;
    Vector4f color;
    Vector2f texCoord;
    Vector3f tangent;
    Vector4f ambOcc;
};

/// \brief Représentation d'une face a nombre variable de vertex

class Face : public Vertex::Array
{
public:
    typedef std::vector<Face> Array;
};

/// \brief Tempon de stockage en mémoire GPU (Vertex buffer object)

class HardwareBuffer
{
public:
    HardwareBuffer();
    ~HardwareBuffer();

    HardwareBuffer(const HardwareBuffer& hb);

    HardwareBuffer & operator=(const HardwareBuffer& hb);

    Vertex* lock(GLenum usage = GL_READ_WRITE);
    void unlock();

    void addFace(const Face& face);

    void addVertex(const Vertex& vertex);
    void addVertex(const Vertex::Array& array);
    void addVertex(const Vertex* array, unsigned size);

    void compile(GLenum usage = GL_STATIC_DRAW);

    void bindBuffer(bool state = true);
    void bindTexture(bool state = true);
    void bindColor(bool state = true);
    void bindNormal(bool state = true);

    void bindTangent(bool state = true, GLint location = -1);
    void bindAocc(bool state = true, GLint location = -1);

    void render(GLenum mode = GL_TRIANGLES, unsigned first = 0, unsigned count = 0);

    bool isEmpty();

    unsigned getBufferSize() const;

    unsigned getVertexCount() const;

    Face::Array getAllFace();

    Vertex::Array getAllVertex(bool makeUnique = false);

    /**
     * Vérifier le support des extention requis
     * par la carte graphique pour le rendue
     *
     * @return
     */
    static bool checkHardware();

protected:
    GLuint m_bufferId;
    GLenum m_usage;
    Vertex::Array m_vertex;

    unsigned m_vertexCount;
    unsigned m_bufferSize;
};

}

#endif	/* _HARDWAREBUFFER_H */
