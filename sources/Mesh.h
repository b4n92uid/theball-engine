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

/**
 * \brief Représentation d'un Mailliage
 * inclus des fonctions de calcules additinelle, de lancé de rayon
 * et d'affictation de materiaux
 */
class Mesh : public Node
{
public:
    Mesh();
    Mesh(const Mesh& mesh);
    ~Mesh();

    bool operator=(const Mesh& mesh);

    Node* Clone();

    /// Rendue
    void Render();

    /// Traitement
    void Process();

    /// Calcule l'AABB du mesh
    void ComputeAabb();

    /// Calcule l'espace tangente des vertex du mesh
    void ComputeTangent();

    /// Calcule l'occlusion ambiante
    void ComputeAocc();

    /// Renvois la position y de coordonnés x z local au mesh
    Vector3f FindFloor(float x, float z);

    /// Renvoi la position d'intersection depuis rayStart vers rayDiri local au mesh
    Vector3f RayCast(Vector3f rayStart, Vector3f rayDiri);

    /// Ajout un materieux au noeud
    void AddMaterial(std::string name, Material* material);

    /// Renvois le nombre de materials assosier au noeud
    unsigned GetMaterialCount();

    /**
     * Renvois tout les application du materieux name
     * sous forme d'un tableau de vecteur ou x=offset et y=lenght
     */
    Vector2i::Array GetMaterialApply(std::string name);

    /// Renvois le materieux identifier par son nom
    Material* GetMaterial(std::string name);

    /// Renvois tout les materials du mesh
    Material::Array GetAllMaterial();

    /// Applique un shader a tout les materieux du mesh
    void ApplyShader(Shader shader);

    /// Applique une couleur a tout les vertecies du mesh
    void ApplyColor(std::string materialName, Vector4f color);

    /// Supprime le materieux name
    void DeleteMaterial(std::string name);

    /// Déttache le materieux name du noeud
    Material* ReleaseMaterial(std::string name);

    /// Contien les coordonnés pour l'application d'une texture
    void SetWithTexCoord(bool withTexCoord);
    bool IsWithTexCoord() const;

    /// Contien les vecteur des normlas pour les faces
    void SetWithNormal(bool withNotmal);
    bool IsWithNormal() const;

    /// Triangulation des face du model
    void SetTriangulate(bool triangulate);
    bool IsTriangulate() const;

    void SetParent(Mesh* parent);
    Mesh*GetParent();

    void AddChild(Mesh* child);

    Mesh* GetChild(unsigned index);

    void ReleaseChild(Mesh* child);
    Mesh* ReleaseChild(unsigned index);

    /**
     * Applique le materieux identifier par name
     * aux vertexs depuis offset jusqu'a offset + size
     *
     * @param name
     * @param offset
     * @param size
     */
    void ApplyMaterial(std::string name, unsigned offset, unsigned size);

    /**
     * Applique le materieux aux vertexs depuis offset jusqu'a offset + size
     *
     * @param material
     * @param offset
     * @param size
     */
    void ApplyMaterial(Material* material, unsigned offset, unsigned size);

    /// Renvoi 'true' si le mesh contien un materieu transparent
    bool IsTransparent();

    /// Renvois le buffer graphique de rendue
    HardwareBuffer& GetHardwareBuffer();

    /**
     * Vérifier le support des extention requis
     * par la carte graphique pour le rendue
     *
     * @return
     */
    static bool CheckHardware();

    typedef std::map<std::string, Mesh*> Map;
    typedef std::vector<Mesh*> Array;

protected:
    bool m_triangulate;
    bool m_withNormal;
    bool m_withTexCoord;

    Mesh* m_parent;
    Mesh::Array m_childs;

    HardwareBuffer m_hardwareBuffer;

private:

    void Render(Material* material, unsigned offset, unsigned size);

    struct RenderProcess
    {
        Mesh* parent;
        std::string applyMaterial;
        unsigned offset;
        unsigned size;

        typedef std::vector<RenderProcess> Array;
    };

    inline static bool RenderProcessSortFunc(const RenderProcess& rp1, const RenderProcess&)
    {
        return !rp1.parent->m_materials[rp1.applyMaterial]->IsTransparent();
    }

    RenderProcess::Array m_renderProess;
    Material::Map m_materials;
};

}
}

#endif	/* _MESH_H */
