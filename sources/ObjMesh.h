#ifndef OBJMESH_H
#define OBJMESH_H

#include "Mesh.h"

namespace tbe
{
namespace scene
{

class Material;
class OBJMesh;
class MeshParallelScene;

/**
 * \brief Loader de fichier MTL pour fichier OBJ (.mtl)
 */
class MTLFile
{
public:
    MTLFile(OBJMesh* parent);
    MTLFile(OBJMesh* parent, const std::string& path);
    MTLFile(const MTLFile& copy);
    ~MTLFile();

    MTLFile & operator=(const MTLFile& copy);

    void open(const std::string& path);

    std::string getFilename() const;

protected:
    std::string m_filename;
    OBJMesh* m_parent;
};

/**
 * \brief Loader de fichier Wavefront OBJ (.obj)
 */
class OBJMesh : public Mesh
{
    friend class MTLFile;

public:
    OBJMesh(MeshParallelScene* scene);
    OBJMesh(MeshParallelScene* scene, const std::string& path);
    OBJMesh(const OBJMesh& org);

    ~OBJMesh();

    OBJMesh* clone();

    OBJMesh & operator=(const OBJMesh& org);

    void open(const std::string& path);

    std::string getFilename() const;

    rtree serialize(std::string root);

    std::vector<std::string> getUsedRessources();

protected:
    OBJMesh& copy(const OBJMesh& copy);

protected:
    std::string m_filename;
    MTLFile m_mtlfile;
};

}
}

#endif
