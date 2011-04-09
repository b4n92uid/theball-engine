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

    void setInFloor(Node* node);

    void setEnableFrustumTest(bool enableFrustumTest);
    bool isEnableFrustumTest() const;

    void setFrustumCullingCount(unsigned frustumCullingCount);
    unsigned getFrustumCullingCount() const;

    AABB getSceneAabb();

private:
    unsigned m_frustumCullingCount;
    bool m_enableFrustumTest;
};

}
}
#endif	/* _MESHPARALLELSCENE_H */
