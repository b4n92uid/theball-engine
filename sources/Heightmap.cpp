#include "Heightmap.h"
#include "MeshParallelScene.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

Heightmap::Heightmap(MeshParallelScene* scene) : Mesh(scene)
{
    m_pixels = NULL;

    m_length = 0;
    m_fileSize = 0;
    m_precision = 2;
    m_rendreId = 0;
}

Heightmap::Heightmap(MeshParallelScene* scene, const std::string& path, unsigned precision) : Mesh(scene)
{
    m_pixels = NULL;

    m_length = 0;
    m_fileSize = 0;
    m_precision = min(1u, precision);
    m_rendreId = 0;

    open(path);
}

Heightmap::~Heightmap()
{
    if(m_pixels)
        delete[] m_pixels;
}

float Heightmap::GetY(unsigned x, unsigned z)
{
    x = min(max(0u, x), m_length - 1);
    z = min(max(0u, z), m_length - 1);

    // RAPPEL
    // On acc�de avec comme calcul z * largeur + x. Ainsi pour trouver l'�l�ment
    // tab [9][4], soit par convention la ligne 9 et colonne 4, cela donne :
    // 4 * m_iLargeur (par exemple 15) : 4 * 15 + 9 = 69.
    return (GLfloat) m_pixels[z * m_length + x];
}

void Heightmap::open(const std::string& path)
{
    m_hardwareBuffer = new HardwareBuffer;

    if(m_pixels);
    delete[] m_pixels;

    FILE * pFile = fopen(path.c_str(), "rb");

    if(!pFile)
        throw tbe::Exception("Heightmap::Open; Open file error (%1%)") % path;

    fseek(pFile, 0, SEEK_END);
    m_fileSize = ftell(pFile);
    m_length = (GLuint) sqrt(m_fileSize);

    rewind(pFile);

    m_pixels = new unsigned char[m_fileSize];
    fread(m_pixels, sizeof (unsigned char), m_fileSize, pFile);

    fclose(pFile);

    unsigned vertexsCount = 0;

    for(GLuint x = 0; x < m_length - m_precision; x += m_precision)
    {
        Face face;

        Vector3f pos3;

        for(GLuint z = 0; z < m_length; z += m_precision)
        {
            Vector3f pos(x, GetY(x, z) / 10.0f, z);
            Vector3f pos2(x + m_precision, GetY(x + m_precision, z) / 10.0f, z);

            Vector3f normal = Vector3f::cross(Vector3f::normalize(pos3), Vector3f::normalize(pos2));
            Vector3f normal2 = Vector3f::cross(Vector3f::normalize(pos), Vector3f::normalize(pos3));

            pos3 = pos;

            Vector2f tex((float) x / m_length * 6, (float) z / m_length * 6);
            Vector2f tex2((float) (x + m_precision) / m_length * 6, (float) z / m_length * 6);

            face.push_back(Vertex(pos, normal, 1, tex));
            face.push_back(Vertex(pos2, normal2, 1, tex2));

            vertexsCount += 2;
        }

        m_hardwareBuffer->addFace(face);
    }

    m_hardwareBuffer->compile();

    computeAabb();

    Material* mainMaterial = MaterialManager::get()->newMaterial("HeightMap");
    mainMaterial->setFaceType(Material::TRIANGLE_STRIP);

    addSubMesh(mainMaterial, 0, vertexsCount);
}
