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

    void SetEnableFrustumTest(bool enableFrustumTest);
    bool IsEnableFrustumTest() const;

    void SetFrustumCullingCount(unsigned frustumCullingCount);
    unsigned GetFrustumCullingCount() const;

protected:
    unsigned m_frustumCullingCount;
    bool m_enableFrustumTest;

};

}
}
#endif	/* _MESHPARALLELSCENE_H */
