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

    void Render();

    /// Renvois la position y des coordonnés x z dans le repere global
    bool FindFloor(Vector3f& pos);

    void SetInFloor(Node* node);

    void SetEnableFrustumTest(bool enableFrustumTest);
    bool IsEnableFrustumTest() const;

    void SetFrustumCullingCount(unsigned frustumCullingCount);
    unsigned GetFrustumCullingCount() const;

    Iterator<Mesh*> GetIterator();

protected:
    AABB getSceneAabb();

private:
    unsigned m_frustumCullingCount;
    bool m_enableFrustumTest;
};

}
}
#endif	/* _MESHPARALLELSCENE_H */
