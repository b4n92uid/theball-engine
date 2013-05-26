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

    /// Rendue
    void render();
    void renderShadow();

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

    /// Ajout un materieux au noeud
    void addMaterial(std::string name, Material* material);

    /// Renvois le nombre de materials assosier au noeud
    unsigned getMaterialCount();

    /**
     * Renvois tout les application du materieux name
     * sous forme d'un tableau de vecteur ou x=offset et y=lenght
     *
     * @param name
     * @return
     */
    Vector2i::Array getMaterialApply(std::string name);

    /// Renvois le materieux identifier par son nom
    Material* getMaterial(std::string name);
    Material* getMaterial(unsigned index);

    /// Renvois tout les materials du mesh
    Material::Array getAllMaterial();

    /// Supprime le materieux name
    void deleteMaterial(std::string name);

    /// Déttache le materieux name du noeud
    Material* releaseMaterial(std::string name);

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
     * Applique le materieux identifier par name
     * aux vertexs depuis offset jusqu'a offset + size
     *
     * @param name
     * @param offset
     * @param size
     */
    void applyMaterial(std::string name, unsigned offset, unsigned size);

    /**
     * Applique le materieux aux vertexs depuis offset jusqu'a offset + size
     *
     * @param material
     * @param offset
     * @param size
     */
    void applyMaterial(Material* material, unsigned offset, unsigned size);

    /// Renvoi `true` si le mesh contien un materieu transparent
    bool isTransparent();

    void setVisible(bool visible);
    bool isVisible() const;

    void setPriorityRender(int priorityRender);
    int getPriorityRender() const;

    /// Renvois le buffer graphique de rendue
    HardwareBuffer* getHardwareBuffer() const;

    rtree serialize(std::string root);

    rtree serializeMaterial(std::string root);

    void attachMaterialSet(const Material::Map& set);
    void attachMaterialFile(std::string path);
    void releaseMaterialFile();
    std::string getMaterialFile();

    typedef std::map<std::string, Mesh*> Map;
    typedef std::vector<Mesh*> Array;

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

    std::vector<std::string> getUsedRessources();

    static void registerBuffer(Mesh* mesh, const std::string& source);
    static void unregisterBuffer(Mesh* mesh);
    static Mesh* isSharedBuffer(const std::string& source);
    static bool isUsedBuffer(HardwareBuffer* hb);

    void requestVertexRestore(bool requestVertexRestore = true);

protected:
    bool m_triangulate;
    bool m_withNormal;
    bool m_withTexCoord;
    bool m_visible;
    bool m_requestVertexRestore;
    bool m_receiveShadow;
    bool m_castShadow;
    bool m_computeNormals;
    bool m_computeTangent;
    bool m_computeAocc;
    int m_priorityRender;

    Vector2b m_billBoard;

    HardwareBuffer* m_hardwareBuffer;

    MeshParallelScene* m_parallelScene;

private:
    void animateTexture(unsigned layer, Texture texture, TextureApply settings);
    void bindTexture(unsigned layer, Texture texture, TextureApply settings);

    void beginRenderingBuffer(Material* material, unsigned, unsigned);
    void beginRenderingProperty(Material* material, unsigned, unsigned);
    void endRenderingProperty(Material* material, unsigned, unsigned);
    void endRenderingBuffer(Material* material, unsigned, unsigned);

    void beginRenderingMatrix();
    void endRenderingMatrix();

    void drawMaterial(Material* material, unsigned offset, unsigned size);

    Shader getUsedShader(Material* material);

    struct RenderProcess
    {
        Mesh* parent;
        std::string applyMaterial;
        unsigned offset;
        unsigned size;

        typedef std::vector<RenderProcess> Array;
    };

    inline static bool renderProcessSortFunc(const RenderProcess& rp1, const RenderProcess&)
    {
        return rp1.parent->m_materials[rp1.applyMaterial]
                ? !rp1.parent->m_materials[rp1.applyMaterial]->isTransparent()
                : false;
    }

    RenderProcess::Array m_renderProess;
    Material::Map m_materials;

    Material::Map m_materialsBackup;
    std::string m_attachMaterial;

    Mesh& copy(const Mesh& copy);
};

}
}

#endif	/* _MESH_H */
