/*
 * File:   HardwareBuffer.h
 * Author: b4n92uid
 *
 * Created on 22 mai 2010, 11:56
 */

#ifndef _HARDWAREBUFFER_H
#define	_HARDWAREBUFFER_H

#include <map>
#include <vector>

#include "GLee.h"

#include "Mathematics.h"
#include "AABB.h"

namespace tbe
{

namespace scene
{
class SubMesh;
}

/// \brief Représentation d'un vertex

class Vertex
{
public:
    Vertex();
    Vertex(Vector3f pos, Vector3f normal, Vector4f color, Vector2f texCoord);
    Vertex(float px, float py, float pz, float nx, float ny, float nz, float r, float g, float b, float a, float s, float t);

    bool operator==(const Vertex& v) const;

    typedef std::vector<Vertex> Array;

public:
    /// Position du vertex
    Vector3f pos;

    /// Normal du vertex
    Vector3f normal;

    /// Couleur du vertex
    Vector4f color;

    /// coordonnées de texture
    Vector2f texCoord;

    /// Tangent du vertex
    Vector3f tangent;

    /// Facteur de l'occlusion ambiante du vertex
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
    HardwareBuffer(const HardwareBuffer& hb);
    ~HardwareBuffer();

    typedef std::vector<unsigned> IndexArray;

    HardwareBuffer & operator=(const HardwareBuffer& hb);

    /// Véroulleu le buffer pour d'éventuelle modification
    Vertex* lock(GLenum usage = GL_READ_WRITE);

    /// Véroulleu le buffer pour d'éventuelle modification
    Vector2f* lockMultiTexCoord(unsigned index, GLenum usage = GL_READ_WRITE);

    /// Dévéroulleu le buffer pour signaler la fin des modification
    HardwareBuffer& unlock();

    /// *
    void restore();

    /// *
    void snapshot();

    /// Ajoute une face au buffer
    void addFace(const Face& face);

    /// Ajoute un vertex au buffer
    void addVertex(const Vertex& vertex);

    /// Ajoute un tablaux de vertex au buffer
    void addVertex(const Vertex::Array& array);

    /// Ajoute un tablaux de vertex au buffer (Style C)
    void addVertex(const Vertex* array, unsigned size);

    void addIndex(unsigned index);

    void addIndex(unsigned* array, unsigned size);

    void convertToIndexedBuffer();

    /**
     * Compile le buffer avec les vertex enregistrer
     *
     * Cette fonction doit etre appeller aprés que tout les ajoute de vertex
     * soit effectuer, si des les vertex sont ajouter aprés l'appelle de cette
     * fonction, ils seront ignorer
     */
    void compile(GLenum usage = GL_STATIC_DRAW);
    void compileIndex(GLenum usage = GL_STATIC_DRAW);

    void clear();

    /// Activer le rendue des position vertex (activation minimum)
    HardwareBuffer& bindBuffer(bool state = true, int vertCount = 3);
    HardwareBuffer& unbindBuffer();

    /// Activer le rendue des coordonnées de texture
    void bindTexture(bool state = true, unsigned layer = 0);

    /// Activer le rendue des couleur associer aux vertexs
    void bindColor(bool state = true);

    /// Activer le pris en compte des normals associer aux vertexs
    void bindNormal(bool state = true);

    /// Activer le pris en compte des tangent associer aux vertexs
    ///     pour le rendue du bump-mapping
    void bindTangent(bool state = true, GLint location = -1);

    /// Activer le pris en compte des de l'ampbient occlusion
    void bindAocc(bool state = true, GLint location = -1);

    /// Rendue du buffer
    void render(GLenum mode = GL_TRIANGLES, unsigned first = 0, unsigned count = 0, int drawcount = 1);

    /// Renvois true si le buffer est compiler et pret au rendue
    bool isEmpty() const;

    /// Renvois la taille du buffer en octet
    unsigned getBufferSize() const;

    /// Renvois la nombre de vertexs contenue dans le buffer
    unsigned getVertexCount() const;

    unsigned getIndexCount() const;

    bool isIndexMode() const;

    /// Renvois un tableau contenant les face triangulaire du buffer
    Face::Array getAllFace();

    /**
     * Renvois un tableau contenant tout les vertexs du buffer
     *
     * @param makeUnique Indique si les vertex qui partagent la méme position
     *                  seront supprimer
     */
    Vertex::Array getAllVertex(bool makeUnique = false);

    const Vertex::Array& getClientVertex() const;
    const IndexArray& getClientIndex() const;

    void setMultiTexCoord(unsigned index, Vector2f::Array uv);
    void newMultiTexCoord(unsigned index);

    /**
     * Vérifier le support des extention requis par la carte graphique pour le rendue
     *
     * Pour déterminer le support des VBO's,
     * l'extention suivante est tester :
     *  GL_ARB_vertex_buffer_object
     */
    static bool checkHardware();

protected:
    GLuint m_bufferId;
    GLuint m_indexId;
    GLenum m_usage;
    Vertex::Array m_vertex;
    IndexArray m_index;

    unsigned m_vertexCount;
    unsigned m_bufferSize;

    unsigned m_indexCount;
    unsigned m_indexBufferSize;

    bool m_indexMode;

    unsigned m_multiTexCoordOffset;
    std::map<unsigned, Vector2f::Array> m_multiTexCoord;

    Vertex* m_cache;
};

}

#endif	/* _HARDWAREBUFFER_H */
