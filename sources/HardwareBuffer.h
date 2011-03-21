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

    Vertex* Lock(GLenum usage = GL_READ_WRITE);
    void UnLock();

    void AddFace(const Face& face);

    void AddVertex(const Vertex& vertex);
    void AddVertex(const Vertex::Array& array);
    void AddVertex(const Vertex* array, unsigned size);

    void Compile(GLenum usage = GL_STATIC_DRAW);

    void BindBuffer(bool state = true);
    void BindTexture(bool state = true);
    void BindColor(bool state = true);
    void BindNormal(bool state = true);

    void BindTangent(bool state = true, GLint location = -1);
    void BindAocc(bool state = true, GLint location = -1);

    void Render(GLenum mode = GL_TRIANGLES, unsigned first = 0, unsigned count = 0);

    bool IsEmpty();

    unsigned GetBufferSize() const;

    unsigned GetVertexCount() const;

    Face::Array GetAllFace();

    Vertex::Array GetAllVertex(bool makeUnique = false);

    /**
     * Vérifier le support des extention requis
     * par la carte graphique pour le rendue
     *
     * @return
     */
    static bool CheckHardware();

protected:
    GLuint m_bufferId;
    GLenum m_usage;
    Vertex::Array m_vertex;

    unsigned m_vertexCount;
    unsigned m_bufferSize;
};

}

#endif	/* _HARDWAREBUFFER_H */
