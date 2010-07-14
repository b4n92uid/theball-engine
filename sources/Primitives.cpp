#include "Primitives.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

// Box -------------------------------------------------------------------------

Box::Box()
{

}

Box::Box(Vector3f size)
{
    Setup(size);
}

void Box::Setup(Vector3f size)
{
    m_size = size;

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
        {
            vertexs[i * 3 + j].pos *= m_size;
            face.push_back(vertexs[i * 3 + j]);
        }

        m_hardwareBuffer.AddFace(face);
    }

    m_hardwareBuffer.Compile();

    ComputeAabb();

    Material* mainMaterial = new Material;
    mainMaterial->SetFaceType(Material::TRIANGLES);

    AddMaterial("main", mainMaterial);
    ApplyMaterial(mainMaterial, 0, 36);
}

Vector3f Box::GetSize() const
{
    return m_size;
}

// Sphere ----------------------------------------------------------------------

Sphere::Sphere()
{
    m_radius = 0;
}

Sphere::Sphere(float radius, unsigned slices, unsigned stackes)
{
    Setup(radius, slices, stackes);
}

void Sphere::Setup(float radius, unsigned slices, unsigned stackes)
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

        m_hardwareBuffer.AddFace(face);
    }

    m_hardwareBuffer.Compile();

    ComputeAabb();

    Material* mainMaterial = new Material;
    mainMaterial->SetFaceType(Material::TRIANGLE_STRIP);

    AddMaterial("main", mainMaterial);
    ApplyMaterial(mainMaterial, 0, m_hardwareBuffer.GetVertexCount());
}

float Sphere::GetRadius() const
{
    return m_radius;
}

// Axes ------------------------------------------------------------------------

Axes::Axes()
{
    m_lineWidth = 0;
    m_lineLength = 0;
}

Axes::Axes(float lineWidth, float lineLength)
{
    Setup(lineWidth, lineLength);
}

void Axes::Setup(float lineWidth, float lineLength)
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

    m_hardwareBuffer.AddFace(faceX);
    m_hardwareBuffer.AddFace(faceY);
    m_hardwareBuffer.AddFace(faceZ);
    m_hardwareBuffer.Compile();

    ComputeAabb();

    Material* mainMaterial = new Material;
    mainMaterial->SetRenderFlags(Material::COLOR);
    mainMaterial->SetLineWidth(m_lineWidth);
    mainMaterial->SetFaceType(Material::LINES);

    AddMaterial("main", mainMaterial);
    ApplyMaterial(mainMaterial, 0, 6);
}

float Axes::GetLineLength() const
{
    return m_lineLength;
}

float Axes::GetLineWidth() const
{
    return m_lineWidth;
}

// Plan ------------------------------------------------------------------------

Plane::Plane()
{

}

Plane::Plane(Vector2f size, Vector2i cut)
{
    Setup(size, cut);
}

void Plane::Setup(Vector2f size, Vector2i cut)
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

            m_hardwareBuffer.AddFace(face);
            m_hardwareBuffer.AddFace(face2);
        }

    m_hardwareBuffer.Compile();

    ComputeAabb();

    Material* mainMaterial = new Material;
    mainMaterial->SetFaceType(Material::TRIANGLES);

    AddMaterial("main", mainMaterial);

    ApplyMaterial(mainMaterial, 0, m_hardwareBuffer.GetVertexCount());
}

Vector2i Plane::GetCut() const
{
    return m_cut;
}

Vector2f Plane::GetSize() const
{
    return m_size;
}
