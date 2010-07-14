#ifndef OBJMESH_H
#define OBJMESH_H

#include "Mesh.h"

namespace tbe
{
namespace scene
{

class Material;
class OBJMesh;

/**
 * \brief Loader de fichier MTL pour fichier OBJ (.mtl)
 */
class MTLFile
{
public:
    MTLFile(OBJMesh* parent);
    MTLFile(OBJMesh* parent, const std::string& path);
    ~MTLFile();

    void Open(const std::string& path);

protected:
    std::string m_filePath;
    OBJMesh* m_parent;
};

/**
 * \brief Loader de fichier Wavefront OBJ (.obj)
 */
class OBJMesh : public Mesh
{
    friend class MTLFile;

public:
    OBJMesh();
    OBJMesh(const std::string& path);
    OBJMesh(const OBJMesh& copy);

    ~OBJMesh();

    OBJMesh & operator=(const OBJMesh& copy);

    void Open(const std::string& path);

    std::string GetFilepath() const;

    typedef std::map<std::string, Node*> Map;

protected:
    std::string m_filepath;
    MTLFile m_mtlfile;
};

}
}

#endif
