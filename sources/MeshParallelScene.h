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
class MeshParallelScene : public ParallelScene
{
public:
    MeshParallelScene();
    ~MeshParallelScene();

    void Render();

    void Clear();

    /// Renvois la position y des coordonnés x z dans le repere global
    float FindFloor(float x, float z);

    /// Renvois le vecteur de position défini pas pos.x, pos.z
    Vector3f FindFloor(Vector3f pos);

    void SetEnableFrustumTest(bool enableFrustumTest);
    bool IsEnableFrustumTest() const;

    void SetFrustumCullingCount(unsigned frustumCullingCount);
    unsigned GetFrustumCullingCount() const;

    void RegisterMesh(Mesh* mesh);
    void UnRegisterMesh(Mesh* mesh, bool deleteptr = false);

    Iterator<Mesh*> GetIterator();

private:
    unsigned m_frustumCullingCount;
    bool m_enableFrustumTest;
    Mesh::Array m_nodes;
};

}
}
#endif	/* _MESHPARALLELSCENE_H */
