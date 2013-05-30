/*
 * File:   MeshParallelScene.h
 * Author: b4n92uid
 *
 * Created on 30 avril 2010, 23:02
 */

#ifndef _MESHPARALLELSCENE_H
#define	_MESHPARALLELSCENE_H

#include "ParallelScene.h"
#include "Mesh.h"
#include "VolumetricLight.h"

namespace tbe
{
namespace scene
{

class Light;

/**
 * \brief Scene parallel de rendue des mailliage
 */
class MeshParallelScene : public ParallelSceneRegister<Mesh>
{
public:
    MeshParallelScene();
    ~MeshParallelScene();

    void render();

    Vector3f::Array rayCast(Vector3f start, Vector3f dir);

    Mesh::Array findMeshs(Vector3f start, Vector3f dir);

    bool findFloor(float x, float& y, float z);
    void setInFloor(Node* node);

    void setEnableFrustumTest(bool enableFrustumTest);
    bool isEnableFrustumTest() const;

    unsigned getFrustumCullingCount() const;
    unsigned getRenderedMeshCount() const;

    void setTransparencySort(bool transparencySort);
    bool isTransparencySort() const;

    AABB getSceneAabb();

    void registerLight(Light* light);
    void unregisterLight(Light* light);

    Light* light(int index);

    int beginPrePassLighting(Mesh* mesh);
    void prePassLighting(int i);
    void endPrePassLighting();

    void setRenderingShader(Shader renderingShader);
    Shader getRenderingShader() const;

    MaterialManager* getMaterialManager() const;

private:
    void drawScene(bool shadowpass = false);

private:
    unsigned m_renderedMeshCount;
    unsigned m_frustumCullingCount;
    bool m_enableFrustumTest;
    bool m_transparencySort;

    Shader m_renderingShader;

    std::vector<Light*> m_lightNodes;
    std::vector<Light*> m_prePassLights;
    MaterialManager* m_materialManager;

    int m_maxlight;
};

}
}
#endif	/* _MESHPARALLELSCENE_H */
