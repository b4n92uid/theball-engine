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

namespace tbe
{
namespace scene
{

/**
 * \brief Scene parallel de rendue des mailliage
 */
class MeshParallelScene : public ParallelSceneRegister<Mesh>
{
public:
    MeshParallelScene();
    ~MeshParallelScene();

    void render();

    /// Renvois la position y des coordonnés x z dans le repere global
    bool findFloor(Vector3f& pos);

    Mesh::Array findMeshs(Vector3f start, Vector3f diri);

    void setInFloor(Node* node);

    void setEnableFrustumTest(bool enableFrustumTest);
    bool isEnableFrustumTest() const;

    void setFrustumCullingCount(unsigned frustumCullingCount);
    unsigned getFrustumCullingCount() const;

    void setTransparencySort(bool transparencySort);
    bool isTransparencySort() const;

    AABB getSceneAabb();

private:
    unsigned m_frustumCullingCount;
    bool m_enableFrustumTest;
    bool m_transparencySort;
};

}
}
#endif	/* _MESHPARALLELSCENE_H */
