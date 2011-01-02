#ifndef _HEIGHTMAP_H
#define _HEIGHTMAP_H

#include "Mesh.h"

namespace tbe
{
namespace scene
{

class MeshParallelScene;

/**
 * \brief Loader de fichier Heightmap (.raw)
 */
class Heightmap : public Mesh
{
public:

    /// Ctor
    Heightmap(MeshParallelScene* scene);

    /// Ctor depuis un fichier
    Heightmap(MeshParallelScene* scene, const std::string& path, unsigned precision = 2);

    /// Dtor
    ~Heightmap();

    /// Construction depuis un fichier
    void Open(const std::string& path);

protected:
    unsigned char* m_pixels;
    unsigned m_length;
    unsigned m_fileSize;
    unsigned m_precision;
    unsigned m_rendreId;

    float GetY(unsigned x, unsigned z);
};

}
}

#endif // _HEIGHTMAP_H
