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

    void CompileStaticMesh();

    void SetEnableFrustumTest(bool enableFrustumTest);
    bool IsEnableFrustumTest() const;

    void SetFrustumCullingCount(unsigned frustumCullingCount);
    unsigned GetFrustumCullingCount() const;

    void AddMesh(std::string name, Mesh* node);

    Mesh* GetMesh(std::string name);

    Mesh* ReleaseMesh(std::string name);

    void DeleteMesh(std::string name);

    template <typename T> T* GetMesh(std::string name)
    {
        return (T*) & GetMesh(name);
    }

    template <typename T> T* ReleaseMesh(std::string name)
    {
        return (T*) & ReleaseMesh(name);
    }

protected:
    unsigned m_frustumCullingCount;
    bool m_enableFrustumTest;
    Mesh::Map m_nodes;

};

}
}
#endif	/* _MESHPARALLELSCENE_H */
