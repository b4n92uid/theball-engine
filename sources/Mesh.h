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
 * \brief Repr�sentation d'un Mailliage
 * inclus des fonctions de calcules additinelle, de lanc� de rayon
 * et d'affictation de materiaux
 */
class Mesh : public Node
{
public:
    Mesh(MeshParallelScene* scene);
    Mesh(const Mesh& copy);
    ~Mesh();

    /// Op�rateur d'assignation
    Mesh & operator=(const Mesh& copy);

    /// M�thode polymorphique de clonage du noueud
    Mesh* clone();

    void clear();

    /// Rendue
    void render();

    /// Traitement
    void process();

    /// Calcule l'AABB du mesh
    void computeAabb();

    /// Renvoi l'AABB Absolue du mesh
    AABB getAbsolutAabb();

    /// Calcule l'espace tangente des vertex du mesh
    void computeTangent();

    /// Calcule l'occlusion ambiante
    void computeAocc();

    /// Renvois la position y des coordonn�s x z dans le repere local du mesh
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

    /// Applique un shader a tout les materieux du mesh
    void applyShader(std::string materialName, Shader shader);
    void applyShader(Shader shader);

    /// Supprime le materieux name
    void deleteMaterial(std::string name);

    /// D�ttache le materieux name du noeud
    Material* releaseMaterial(std::string name);

    /// Contien les coordonn�s pour l'application d'une texture
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

    /// Renvois le buffer graphique de rendue
    HardwareBuffer* getHardwareBuffer() const;

    CtorMap constructionMap(std::string root);

    void generateMulTexCoord();

    typedef std::map<std::string, Mesh*> Map;
    typedef std::vector<Mesh*> Array;

    void setBillBoard(Vector2b billBoard);
    Vector2b getBillBoard() const;

    bool rayCast(Vector3f rayStart, Vector3f rayDiri, float& intersect, bool global);

    void sahreFrom(const Mesh& copy);
    void fetchMaterials(const Mesh& copy);

    std::vector<std::string> getUsedRessources();

    static void registerBuffer(Mesh* mesh, const std::string& source);
    static void unregisterBuffer(Mesh* mesh);
    static Mesh* isSharedBuffer(const std::string& source);
    static bool isUsedBuffer(HardwareBuffer* hb);

    void setOutputMaterial(bool outputMaterial);
    bool isOutputMaterial() const;

    void requestVertexRestore(bool requestVertexRestore = true);

protected:
    CtorMap outputMaterial(std::string root);

protected:
    bool m_triangulate;
    bool m_withNormal;
    bool m_withTexCoord;
    bool m_visible;
    bool m_outputMaterial;
    bool m_requestVertexRestore;

    Vector2b m_billBoard;

    HardwareBuffer* m_hardwareBuffer;

    MeshParallelScene* m_parallelScene;

private:
    void beginRenderingMaterials(Material* material, unsigned, unsigned);
    void setupMaterialsProperty(Material* material, unsigned, unsigned);
    void unsetupMaterialsProperty(Material* material, unsigned, unsigned);
    void endRenderingMaterials(Material* material, unsigned, unsigned);
    
    void beginRenderingMatrix();
    void endRenderingMatrix();

    void render(Material* material, unsigned offset, unsigned size);

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
        return !rp1.parent->m_materials[rp1.applyMaterial]->isTransparent();
    }

    RenderProcess::Array m_renderProess;
    Material::Map m_materials;

    Mesh& copy(const Mesh& copy);

    GLint m_tangentAttribIndex;
    GLint m_aoccAttribIndex;
};

}
}

#endif	/* _MESH_H */
