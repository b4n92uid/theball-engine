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

    /// Renvois la position y des coordonnés x z dans le repere local du mesh
    bool findFloor(Vector3f& pos, bool global);

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

    /// Renvois tout les materials du mesh
    Material::Array getAllMaterial();

    /// Applique un shader a tout les materieux du mesh
    void applyShader(std::string materialName, Shader shader);
    void applyShader(Shader shader);

    /// Applique une couleur a tout les vertecies du mesh
    void setColor(Vector4f color);
    Vector4f getColor();

    void setOpacity(float opacity);
    float getOpacity() const;

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

    /// Renvois le buffer graphique de rendue
    HardwareBuffer* getHardwareBuffer() const;

    CtorMap constructionMap(std::string root);

    void setOutputMaterial(bool outputMaterial);
    bool isOutputMaterial() const;

    void setVertexScale(Vector3f vertexScale);
    Vector3f getVertexScale() const;

    void generateMulTexCoord();

    typedef std::map<std::string, Mesh*> Map;
    typedef std::vector<Mesh*> Array;

    void setBillBoard(Vector2b billBoard);
    Vector2b getBillBoard() const;

    bool rayCast(Vector3f rayStart, Vector3f rayDiri,
                 Vector3f& intersect, bool global);

    void fetch(const Mesh& copy);

    virtual std::vector<std::string> getUsedRessources();
    
    static void registerBuffer(Mesh* mesh, const std::string& source);
    static void unregisterBuffer(Mesh* mesh);
    static Mesh* isSharedBuffer(const std::string& source);
    static bool isUsedBuffer(HardwareBuffer* hb);

protected:
    bool m_triangulate;
    bool m_withNormal;
    bool m_withTexCoord;
    bool m_visible;
    bool m_outputMaterial;

    HardwareBuffer* m_hardwareBuffer;

    MeshParallelScene* m_parallelScene;

private:
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

    Vector2b m_billBoard;
    Vector4f m_color;
    Vector3f m_vertexScale;
};

}
}

#endif	/* _MESH_H */
