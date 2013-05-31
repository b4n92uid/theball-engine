/*
 * File:   Mesh.h
 * Author: b4n92uid
 *
 * Created on 28 avril 2010, 18:36
 */

#ifndef _MESH_H
#define	_MESH_H

#include "Node.h"
#include "Material.h"

namespace tbe
{
namespace scene
{
class MeshParallelScene;
class Mesh;

class SubMesh
{
public:
    SubMesh(Mesh*, Material*, unsigned, unsigned);
    ~SubMesh();

    void setMaterial(Material* material);
    Material* getMaterial() const;

    void setSize(unsigned size);
    unsigned getSize() const;

    void setOffset(unsigned offset);
    unsigned getOffset() const;

    void bindBuffers();
    void beginProperty();
    void draw(const Matrix4& mat);
    void endProperty();
    void unbindBuffers();

    void beginShadowPass();
    void drawShadow(const Matrix4& mat);
    void endShadowPass();

    void setOwner(Mesh* owner);
    Mesh* getOwner() const;

    bool operator==(const SubMesh& other);

    typedef std::vector<SubMesh*> Array;

    friend class Mesh;

private:
    void transform(const Matrix4& mat);
    void animateTexture(unsigned layer, Texture texture, TextureApply settings);
    void bindTexture(unsigned layer, Texture texture, TextureApply settings);

private:
    Mesh* m_owner;
    Material* m_material;
    unsigned m_offset;
    unsigned m_size;

    HardwareBuffer* m_hardbuf;
    SceneManager* m_scenemng;
    MeshParallelScene* m_parallel;
};

/**
 * \brief Représentation d'un Mailliage
 * inclus des fonctions de calcules additinelle, de lancé de rayon
 * et d'affictation de materiaux
 */
class Mesh : public Node
{
public:
    Mesh(MeshParallelScene* scene);
    Mesh(const Mesh& copy);
    ~Mesh();

    /// Opérateur d'assignation
    Mesh & operator=(const Mesh& copy);

    /// Méthode polymorphique de clonage du noueud
    Mesh* clone();

    void clear();

    /// Traitement
    void process();

    /// Calcule l'AABB du mesh
    void computeAabb();

    /// Renvoi l'AABB Absolue du mesh
    AABB getAbsolutAabb();

    /// Calcule l'espace tangente des vertex du mesh
    void computeTangent();

    /// Calcule les normals
    void computeNormal();

    /// Calcule l'occlusion ambiante
    void computeAocc();

    bool isComputeAocc() const;
    bool isComputeTangent() const;
    bool isComputeNormals() const;

    /// Renvois la position y des coordonnés x z dans le repere local du mesh
    bool findFloor(float getx, float& sety, float getz, bool global);

    /// Contien les coordonnés pour l'application d'une texture
    void setWithTexCoord(bool withTexCoord);
    bool isWithTexCoord() const;

    /// Contien les vecteur des normlas pour les faces
    void setWithNormal(bool withNotmal);
    bool isWithNormal() const;

    /// Triangulation des face du model
    void setTriangulate(bool triangulate);
    bool isTriangulate() const;

    /**
     * Applique le materieux aux vertexs depuis offset jusqu'a offset + size
     *
     * @param material
     * @param offset
     * @param size
     */
    SubMesh* addSubMesh(Material* material, unsigned offset, unsigned size);

    SubMesh* getSubMesh(int index);
    SubMesh* getSubMesh(std::string matname);

    const SubMesh::Array& getAllSubMesh();

    /// Renvois le nombre de materials assosier au noeud
    unsigned getSubMeshCount();

    /// Renvoi `true` si le mesh contien un materieu transparent
    bool isTransparent();

    void setVisible(bool visible);
    bool isVisible() const;

    void setPriorityRender(int priorityRender);
    int getPriorityRender() const;

    /// Renvois le buffer graphique de rendue
    HardwareBuffer* getHardwareBuffer() const;

    rtree serialize(std::string root);

    void setBillBoard(Vector2b billBoard);
    Vector2b getBillBoard() const;

    void setCastShadow(bool castShadow);
    bool isCastShadow() const;

    void setReceiveShadow(bool receiveShadow);
    bool isReceiveShadow() const;

    bool rayCast(Vector3f rayStart, Vector3f rayDiri, float& intersect, bool global);

    void shareVertexes(const Mesh& copy);
    void fetchVertexes(const Mesh& copy);
    void fetchMaterials(const Mesh& copy);

    MeshParallelScene* getParallelScene() const;

    std::vector<std::string> getUsedRessources();

    static void registerBuffer(Mesh* mesh, const std::string& source);
    static void unregisterBuffer(Mesh* mesh);
    static Mesh* isSharedBuffer(const std::string& source);
    static bool isUsedBuffer(HardwareBuffer* hb);

    typedef std::map<std::string, Mesh*> Map;
    typedef std::vector<Mesh*> Array;

protected:
    bool m_triangulate;
    bool m_withNormal;
    bool m_withTexCoord;
    bool m_visible;
    bool m_receiveShadow;
    bool m_castShadow;
    bool m_computeNormals;
    bool m_computeTangent;
    bool m_computeAocc;
    int m_priorityRender;

    Vector2b m_billBoard;

    HardwareBuffer* m_hardwareBuffer;

    SubMesh::Array m_subMeshs;

    MeshParallelScene* m_parallelScene;

private:
    bool rayCastVertexBuffer(Vector3f rayStart, Vector3f rayDiri, float& intersect, bool global);
    bool rayCastIndexBuffer(Vector3f rayStart, Vector3f rayDiri, float& intersect, bool global);
    Mesh& copy(const Mesh& copy);
};

}
}

#endif	/* _MESH_H */
