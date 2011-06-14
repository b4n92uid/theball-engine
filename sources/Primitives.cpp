#include "Primitives.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

// Box -------------------------------------------------------------------------

Box::Box(MeshParallelScene* scene) : Mesh(scene)
{

}

Box::Box(MeshParallelScene* scene, Vector3f size) : Mesh(scene)
{
    setup(size);
}

void Box::setup(Vector3f size)
{
    /*
    Vertex vertexs[24];

    // Front
    vertexs[ 0] = Vertex(-1, -1, +1, 0, 0, 1, 1, 1, 1, 1, 1, 0);
    vertexs[ 1] = Vertex(+1, -1, +1, 0, 0, 1, 1, 1, 1, 1, 1, 0);
    vertexs[ 2] = Vertex(+1, +1, +1, 0, 0, 1, 1, 1, 1, 1, 1, 0);
    vertexs[ 3] = Vertex(-1, +1, +1, 0, 0, 1, 1, 1, 1, 1, 1, 0);
    // Back
    vertexs[ 4] = Vertex(+1, -1, -1, 0, 0, -1, 1, 1, 1, 1, 1, 0);
    vertexs[ 5] = Vertex(-1, -1, -1, 0, 0, -1, 1, 1, 1, 1, 1, 0);
    vertexs[ 6] = Vertex(-1, +1, -1, 0, 0, -1, 1, 1, 1, 1, 1, 0);
    vertexs[ 7] = Vertex(+1, +1, -1, 0, 0, -1, 1, 1, 1, 1, 1, 0);
    // Left
    vertexs[12] = Vertex(+1, -1, -1, 1, 0, 1, 1, 1, 1, 1, 1, 0);
    vertexs[13] = Vertex(+1, +1, -1, 1, 0, 1, 1, 1, 1, 1, 1, 0);
    vertexs[14] = Vertex(+1, +1, +1, 1, 0, 1, 1, 1, 1, 1, 1, 0);
    vertexs[15] = Vertex(+1, -1, +1, 1, 0, 1, 1, 1, 1, 1, 1, 0);
    // Right
    vertexs[ 8] = Vertex(-1, -1, -1, -1, 0, 0, 1, 1, 1, 1, 1, 0);
    vertexs[ 9] = Vertex(-1, -1, +1, -1, 0, 0, 1, 1, 1, 1, 1, 0);
    vertexs[10] = Vertex(-1, +1, +1, -1, 0, 0, 1, 1, 1, 1, 1, 0);
    vertexs[11] = Vertex(-1, +1, -1, -1, 0, 0, 1, 1, 1, 1, 1, 0);
    // Top
    vertexs[16] = Vertex(+1, +1, -1, 0, 1, 0, 1, 1, 1, 1, 1, 0);
    vertexs[17] = Vertex(-1, +1, -1, 0, 1, 0, 1, 1, 1, 1, 1, 0);
    vertexs[18] = Vertex(-1, +1, +1, 0, 1, 0, 1, 1, 1, 1, 1, 0);
    vertexs[19] = Vertex(+1, +1, +1, 0, 1, 0, 1, 1, 1, 1, 1, 0);
    // Bottom
    vertexs[20] = Vertex(+1, -1, -1, 0, -1, 0, 1, 1, 1, 1, 1, 0);
    vertexs[21] = Vertex(+1, -1, +1, 0, -1, 0, 1, 1, 1, 1, 1, 0);
    vertexs[22] = Vertex(-1, -1, +1, 0, -1, 0, 1, 1, 1, 1, 1, 0);
    vertexs[23] = Vertex(-1, -1, -1, 0, -1, 0, 1, 1, 1, 1, 1, 0);

    for(unsigned i = 0; i < 6; i++)
    {
        Face face;

        for(unsigned j = 0; j < 4; j++)
        {
            vertexs[i * 4 + j].pos *= m_size;
            face.push_back(vertexs[i * 4 + j]);
        }

        m_hardwareBuffer.addFace(face);
    }

    m_hardwareBuffer.compile();

    computeAabb();

    Material* mainMaterial = new Material;
    mainMaterial->setFaceType(Material::QUADS);

    addMaterial("main", mainMaterial);
    applyMaterial(mainMaterial, 0, 24);
     */

    Vertex vertexs[36];

    // Front
    vertexs[0] = Vertex(-1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0);
    vertexs[1] = Vertex(-1, -1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0);
    vertexs[2] = Vertex(1, -1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1);
    vertexs[3] = Vertex(1, -1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1);
    vertexs[4] = Vertex(1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1);
    vertexs[5] = Vertex(-1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0);
    // Back
    vertexs[6] = Vertex(1, 1, -1, 0, 0, -1, 1, 1, 1, 1, 1, 0);
    vertexs[7] = Vertex(1, -1, -1, 0, 0, -1, 1, 1, 1, 1, 0, 0);
    vertexs[8] = Vertex(-1, -1, -1, 0, 0, -1, 1, 1, 1, 1, 0, 1);
    vertexs[9] = Vertex(-1, -1, -1, 0, 0, -1, 1, 1, 1, 1, 0, 1);
    vertexs[10] = Vertex(-1, 1, -1, 0, 0, -1, 1, 1, 1, 1, 1, 1);
    vertexs[11] = Vertex(1, 1, -1, 0, 0, -1, 1, 1, 1, 1, 1, 0);
    // Top
    vertexs[14] = Vertex(1, 1, -1, 0, 1, 0, 1, 1, 1, 1, 1, 0);
    vertexs[13] = Vertex(1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0);
    vertexs[12] = Vertex(-1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1);
    vertexs[17] = Vertex(-1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1);
    vertexs[16] = Vertex(-1, 1, -1, 0, 1, 0, 1, 1, 1, 1, 1, 1);
    vertexs[15] = Vertex(1, 1, -1, 0, 1, 0, 1, 1, 1, 1, 1, 0);
    // Bottom
    vertexs[20] = Vertex(1, -1, 1, 0, -1, 0, 1, 1, 1, 1, 1, 0);
    vertexs[19] = Vertex(1, -1, -1, 0, -1, 0, 1, 1, 1, 1, 0, 0);
    vertexs[18] = Vertex(-1, -1, -1, 0, -1, 0, 1, 1, 1, 1, 0, 1);
    vertexs[23] = Vertex(-1, -1, -1, 0, -1, 0, 1, 1, 1, 1, 0, 1);
    vertexs[22] = Vertex(-1, -1, 1, 0, -1, 0, 1, 1, 1, 1, 1, 1);
    vertexs[21] = Vertex(1, -1, 1, 0, -1, 0, 1, 1, 1, 1, 1, 0);
    // Left
    vertexs[25] = Vertex(-1, 1, -1, -1, 0, 0, 1, 1, 1, 1, 0, 0);
    vertexs[26] = Vertex(-1, -1, -1, -1, 0, 0, 1, 1, 1, 1, 1, 0);
    vertexs[24] = Vertex(-1, -1, 1, -1, 0, 0, 1, 1, 1, 1, 1, 1);
    vertexs[28] = Vertex(-1, -1, 1, -1, 0, 0, 1, 1, 1, 1, 1, 1);
    vertexs[29] = Vertex(-1, 1, 1, -1, 0, 0, 1, 1, 1, 1, 0, 1);
    vertexs[27] = Vertex(-1, 1, -1, -1, 0, 0, 1, 1, 1, 1, 0, 0);
    // Right
    vertexs[30] = Vertex(1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1);
    vertexs[31] = Vertex(1, -1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0);
    vertexs[32] = Vertex(1, -1, -1, 1, 0, 0, 1, 1, 1, 1, 1, 0);
    vertexs[33] = Vertex(1, -1, -1, 1, 0, 0, 1, 1, 1, 1, 1, 0);
    vertexs[34] = Vertex(1, 1, -1, 1, 0, 0, 1, 1, 1, 1, 1, 1);
    vertexs[35] = Vertex(1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1);

    for(unsigned i = 0; i < 12; i++)
    {
        Face face;

        for(unsigned j = 0; j < 3; j++)
            face.push_back(vertexs[i * 3 + j]);

        m_hardwareBuffer.addFace(face);
    }

    m_hardwareBuffer.compile();

    setSize(size);

    Material* mainMaterial = new Material;
    mainMaterial->setFaceType(Material::TRIANGLES);

    addMaterial("main", mainMaterial);
    applyMaterial(mainMaterial, 0, 36);
}

void Box::setSize(Vector3f size)
{
    m_size = size;

    m_aabb.clear();

    Vertex* verts = m_hardwareBuffer.lock();

    for(unsigned i = 0; i < 36; i++)
    {
        verts[i].pos.reSet(m_size);
        m_aabb.count(verts[i].pos);
    }

    m_hardwareBuffer.unlock();
}

Vector3f Box::getSize() const
{
    return m_size;
}

// Sphere ----------------------------------------------------------------------

Sphere::Sphere(MeshParallelScene* scene) : Mesh(scene)
{
    m_radius = 0;
}

Sphere::Sphere(MeshParallelScene* scene, float radius, unsigned slices, unsigned stackes) : Mesh(scene)
{
    setup(radius, slices, stackes);
}

void Sphere::setup(float radius, unsigned slices, unsigned stackes)
{
    m_radius = radius;

    float fHorizontal = M_PI / slices;
    float fVertical = 2.0f * (M_PI / stackes);

    // Parcours horizontal
    for(unsigned int i = 0; i < slices; i++)
    {
        // Cercle horizontal courant
        float fPointCourantH = static_cast<float>(i) * fHorizontal;
        // Cercle horizontal suivant
        float fPointSuivantH = fPointCourantH + fHorizontal;

        // Positions en X
        float fPointCourantX = sinf(fPointCourantH) * m_radius;
        float fPointSuivantX = sinf(fPointSuivantH) * m_radius;

        // Positions en Z
        float fPointCourantZ = cosf(fPointCourantH) * m_radius;
        float fPointSuivantZ = cosf(fPointSuivantH) * m_radius;

        Face face;

        // Parcours vertical
        for(unsigned int j = 0; j <= stackes; j++)
        {
            float fPointCourantV = static_cast<float>(j) * fVertical;
            float x = cosf(fPointCourantV);
            float y = sinf(fPointCourantV);

            Vertex vert1(x * fPointCourantX, y * fPointCourantX, fPointCourantZ,
                         x * fPointCourantX, y * fPointCourantX, fPointCourantZ,
                         1, 1, 1, 1,
                         (float)j / (float)stackes, (float)i / (float)slices);

            face.push_back(vert1);

            Vertex vert2(x * fPointSuivantX, y * fPointSuivantX, fPointSuivantZ,
                         x * fPointSuivantX, y * fPointSuivantX, fPointSuivantZ,
                         1, 1, 1, 1,
                         (float)j / (float)stackes, (float)(i + 1) / (float)slices);

            face.push_back(vert2);

        }

        m_hardwareBuffer.addFace(face);
    }

    m_hardwareBuffer.compile();

    computeAabb();

    Material* mainMaterial = new Material;
    mainMaterial->setFaceType(Material::TRIANGLE_STRIP);

    addMaterial("main", mainMaterial);
    applyMaterial(mainMaterial, 0, m_hardwareBuffer.getVertexCount());
}

float Sphere::getRadius() const
{
    return m_radius;
}

// Axes ------------------------------------------------------------------------

Axes::Axes(MeshParallelScene* scene) : Mesh(scene)
{
    m_lineWidth = 0;
    m_lineLength = 0;
}

Axes::Axes(MeshParallelScene* scene, float lineWidth, float lineLength) : Mesh(scene)
{
    setup(lineWidth, lineLength);
}

void Axes::setup(float lineWidth, float lineLength)
{
    m_lineWidth = lineWidth;
    m_lineLength = lineLength;

    Face faceX, faceY, faceZ;

    faceX.push_back(Vertex(Vector3f(0, 0, 0), 0.0, Vector4f(1, 0, 0, 1), 0.0));
    faceX.push_back(Vertex(Vector3f(m_lineLength, 0, 0), 0.0, Vector4f(1, 0, 0, 1), 0.0));

    faceY.push_back(Vertex(Vector3f(0, 0, 0), 0.0, Vector4f(0, 1, 0, 1), 0.0));
    faceY.push_back(Vertex(Vector3f(0, m_lineLength, 0), 0.0, Vector4f(0, 1, 0, 1), 0.0));

    faceZ.push_back(Vertex(Vector3f(0, 0, 0), 0.0, Vector4f(0, 0, 1, 1), 0.0));
    faceZ.push_back(Vertex(Vector3f(0, 0, m_lineLength), 0.0, Vector4f(0, 0, 1, 1), 0.0));

    m_hardwareBuffer.addFace(faceX);
    m_hardwareBuffer.addFace(faceY);
    m_hardwareBuffer.addFace(faceZ);
    m_hardwareBuffer.compile();

    computeAabb();

    Material* mainMaterial = new Material;
    mainMaterial->setRenderFlags(Material::COLORED);
    mainMaterial->setLineWidth(m_lineWidth);
    mainMaterial->setFaceType(Material::LINES);

    addMaterial("main", mainMaterial);
    applyMaterial(mainMaterial, 0, 6);
}

float Axes::getLineLength() const
{
    return m_lineLength;
}

float Axes::getLineWidth() const
{
    return m_lineWidth;
}

// Plan ------------------------------------------------------------------------

Plane::Plane(MeshParallelScene* scene) : Mesh(scene)
{

}

Plane::Plane(MeshParallelScene* scene, Vector2f size, Vector2i cut) : Mesh(scene)
{
    setup(size, cut);
}

void Plane::setup(Vector2f size, Vector2i cut)
{
    Vector2f quadSize = size / Vector2f(cut);

    for(int x = 0; x < cut.x; x++)
        for(int y = 0; y < cut.y; y++)
        {
            Vertex vertextes[6];

            // 0,1
            vertextes[0] = Vertex(Vector3f(quadSize.x * (x + 1), 0, -quadSize.y * (y + 1)),
                                  Vector3f(0, 1, 0),
                                  1,
                                  Vector2f(0 / (x + 1), 1 / (y + 1)));

            // 0,0
            vertextes[1] = Vertex(Vector3f(quadSize.x * (x + 1), 0, quadSize.y * (y + 1)),
                                  Vector3f(0, 1, 0),
                                  1,
                                  Vector2f(0 / (x + 1), 0 / (y + 1)));

            // 1,0
            vertextes[2] = Vertex(Vector3f(-quadSize.x * (x + 1), 0, quadSize.y * (y + 1)),
                                  Vector3f(0, 1, 0),
                                  1,
                                  Vector2f(1 / (x + 1), 0 / (y + 1)));

            // 1,0
            vertextes[3] = Vertex(Vector3f(-quadSize.x * (x + 1), 0, quadSize.y * (y + 1)),
                                  Vector3f(0, 1, 0),
                                  1,
                                  Vector2f(1 / (x + 1), 0 / (y + 1)));

            // 1,1
            vertextes[4] = Vertex(Vector3f(-quadSize.x * (x + 1), 0, -quadSize.y * (y + 1)),
                                  Vector3f(0, 1, 0),
                                  1,
                                  Vector2f(1 / (x + 1), 1 / (y + 1)));

            // 0,1
            vertextes[5] = Vertex(Vector3f(quadSize.x * (x + 1), 0, -quadSize.y * (y + 1)),
                                  Vector3f(0, 1, 0),
                                  1,
                                  Vector2f(0 / (x + 1), 1 / (y + 1)));

            Face face, face2;

            for(int i = 0; i < 3; i++)
            {
                face.push_back(vertextes[i]);
                face2.push_back(vertextes[i + 3]);
            }

            m_hardwareBuffer.addFace(face);
            m_hardwareBuffer.addFace(face2);
        }

    m_hardwareBuffer.compile();

    computeAabb();

    Material* mainMaterial = new Material;
    mainMaterial->setFaceType(Material::TRIANGLES);

    addMaterial("main", mainMaterial);

    applyMaterial(mainMaterial, 0, m_hardwareBuffer.getVertexCount());
}

Vector2i Plane::getCut() const
{
    return m_cut;
}

Vector2f Plane::getSize() const
{
    return m_size;
}

// Grid ------------------------------------------------------------------------

Grid::Grid(MeshParallelScene* scene) : Mesh(scene)
{

}

Grid::Grid(MeshParallelScene* scene, Vector2f size, Vector2i cut) : Mesh(scene)
{
    setup(size, cut);
}

void Grid::setup(Vector2f size, Vector2i cut)
{
    Vector2f quadSize = size / Vector2f(cut);

    for(int x = -cut.x; x <= cut.x; x++)
    {
        Vertex vertexs[2];

        // 0,0
        vertexs[1] = Vertex(Vector3f(quadSize.x * x, 0, -size.y),
                            Vector3f(0, 1, 0), 1, 0);

        // 0,1
        vertexs[0] = Vertex(Vector3f(quadSize.x * x, 0, size.y),
                            Vector3f(0, 1, 0), 1, 0);

        m_hardwareBuffer.addVertex(vertexs, 2);
    }

    for(int y = -cut.y; y <= cut.y; y++)
    {
        Vertex vertexs[2];

        // 0,0
        vertexs[1] = Vertex(Vector3f(-size.x, 0, quadSize.y * y),
                            Vector3f(0, 1, 0), 1, 0);

        // 0,1
        vertexs[0] = Vertex(Vector3f(size.x, 0, quadSize.y * y),
                            Vector3f(0, 1, 0), 1, 0);

        m_hardwareBuffer.addVertex(vertexs, 2);
    }

    m_hardwareBuffer.compile();

    computeAabb();

    Material* mainMaterial = new Material;
    mainMaterial->disable(Material::TEXTURED | Material::LIGHTED);
    mainMaterial->setFaceType(Material::LINES);
    mainMaterial->setDrawPass(m_hardwareBuffer.getVertexCount() / 2);

    addMaterial("main", mainMaterial);

    applyMaterial(mainMaterial, 0, m_hardwareBuffer.getVertexCount());
}

Vector2i Grid::getCut() const
{
    return m_cut;
}

Vector2f Grid::getSize() const
{
    return m_size;
}
