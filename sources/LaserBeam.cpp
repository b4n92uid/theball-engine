/*
 * File:   LaserBeam.cpp
 * Author: b4n92uid
 *
 * Created on 18 janvier 2013, 20:46
 */

#include "LaserBeam.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

LaserBeam::LaserBeam(MeshParallelScene* scene) : Mesh(scene)
{
    m_opacity = 1;
}

LaserBeam::~LaserBeam()
{
}

void LaserBeam::shoot(Vector3f start, Vector3f end)
{
    m_start = start;
    m_end = end;

    // Build visual ------------------------------------------------------------

    if(!m_hardwareBuffer)
        m_hardwareBuffer = new HardwareBuffer;
    else
        m_hardwareBuffer->clear();

    Vertex vertexs[24];

    // ---
    vertexs[0] = Vertex(1, 0, 1, 0, -1, 0, 1, 1, 1, 1, 1, 0);
    vertexs[1] = Vertex(1, 0, -1, 0, -1, 0, 1, 1, 1, 1, 0, 0);
    vertexs[2] = Vertex(-1, 0, -1, 0, -1, 0, 1, 1, 1, 1, 0, 1);
    vertexs[3] = Vertex(-1, 0, -1, 0, -1, 0, 1, 1, 1, 1, 0, 1);
    vertexs[4] = Vertex(-1, 0, 1, 0, -1, 0, 1, 1, 1, 1, 1, 1);
    vertexs[5] = Vertex(1, 0, 1, 0, -1, 0, 1, 1, 1, 1, 1, 0);
    // |
    vertexs[6] = Vertex(0, 1, -1, -1, 0, 0, 1, 1, 1, 1, 1, 1);
    vertexs[7] = Vertex(0, -1, -1, -1, 0, 0, 1, 1, 1, 1, 1, 0);
    vertexs[8] = Vertex(0, -1, 1, -1, 0, 0, 1, 1, 1, 1, 0, 0);
    vertexs[9] = Vertex(0, -1, 1, -1, 0, 0, 1, 1, 1, 1, 0, 0);
    vertexs[10] = Vertex(0, 1, 1, -1, 0, 0, 1, 1, 1, 1, 0, 1);
    vertexs[11] = Vertex(0, 1, -1, -1, 0, 0, 1, 1, 1, 1, 1, 1);
    // -/-
    vertexs[12] = Vertex(1, 1, -1, -1, 0, 0, 1, 1, 1, 1, 1, 1);
    vertexs[13] = Vertex(-1, -1, -1, -1, 0, 0, 1, 1, 1, 1, 1, 0);
    vertexs[14] = Vertex(-1, -1, 1, -1, 0, 0, 1, 1, 1, 1, 0, 0);
    vertexs[15] = Vertex(-1, -1, 1, -1, 0, 0, 1, 1, 1, 1, 0, 0);
    vertexs[16] = Vertex(1, 1, 1, -1, 0, 0, 1, 1, 1, 1, 0, 1);
    vertexs[17] = Vertex(1, 1, -1, -1, 0, 0, 1, 1, 1, 1, 1, 1);
    // -\-
    vertexs[18] = Vertex(-1, 1, -1, -1, 0, 0, 1, 1, 1, 1, 1, 1);
    vertexs[19] = Vertex(1, -1, -1, -1, 0, 0, 1, 1, 1, 1, 1, 0);
    vertexs[20] = Vertex(1, -1, 1, -1, 0, 0, 1, 1, 1, 1, 0, 0);
    vertexs[21] = Vertex(1, -1, 1, -1, 0, 0, 1, 1, 1, 1, 0, 0);
    vertexs[22] = Vertex(-1, 1, 1, -1, 0, 0, 1, 1, 1, 1, 0, 1);
    vertexs[23] = Vertex(-1, 1, -1, -1, 0, 0, 1, 1, 1, 1, 1, 1);

    for(unsigned i = 0; i < 8; i++)
    {
        Face face;

        for(unsigned j = 0; j < 3; j++)
        {
            face.push_back(vertexs[i * 3 + j]);
        }

        m_hardwareBuffer->addFace(face);
    }

    Material* mainMaterial = new Material;
    mainMaterial->disable(Material::LIGHTED);
    mainMaterial->enable(Material::TEXTURED | Material::ADDITIVE | Material::COLORED);
    mainMaterial->setFaceType(Material::TRIANGLES);
    mainMaterial->setTexture(m_texture);

    addMaterial("main", mainMaterial);
    applyMaterial(mainMaterial, 0, 24);

    m_hardwareBuffer->compile();

    // Rotate and translate ----------------------------------------------------

    m_matrix.identity();

    {
        float length = (end - start).getMagnitude();

        Vertex* verts = m_hardwareBuffer->bindBuffer().lock();

        for(unsigned i = 0; i < 24; i++)
        {
            verts[i].pos.z *= length / 2;
        }

        m_hardwareBuffer->snapshot();

        m_hardwareBuffer->unlock().unbindBuffer();
    }

    // Angle between vectors from:
    // http://www.euclideanspace.com/maths/algebra/vectors/angleBetween/index.htm

    Vector3f v1 = Vector3f(0, 0, 1);
    Vector3f v2 = Vector3f::normalize(end - start);

    float d = Vector3f::dot(v1, v2);

    Vector3f axis = Vector3f::cross(v2, v1);

    float qw = sqrt(v1.getMagnitude() * v2.getMagnitude()) + d;

    Quaternion q;

    if(qw < 0.0001)
    {
        // vectors are 180 degrees apart
        q = Quaternion(0, -v1.z, v1.y, v1.x);
    }
    else
    {
        q = Quaternion(qw, axis.x, axis.y, axis.z);
    }

    q.normalize();

    m_matrix.rotate(q);

    m_matrix.translate(start + (end - start) / 2);
}

void LaserBeam::setTexture(Texture texture)
{
    this->m_texture = texture;

    Material* mainMaterial = getMaterial("main");

    if(mainMaterial)
        mainMaterial->setTexture(m_texture);
}

Texture LaserBeam::getTexture() const
{
    return m_texture;
}

void LaserBeam::setOpacity(float opacity)
{
    this->m_opacity = opacity;

    if(m_hardwareBuffer->isEmpty())
        return;

    Vertex* verts = m_hardwareBuffer->bindBuffer().lock();

    for(unsigned i = 0; i < 24; i++)
    {
        verts[i].color.w = m_opacity;
    }

    m_hardwareBuffer->unlock().unbindBuffer();
}

float LaserBeam::getOpacity() const
{
    return m_opacity;
}

Vector3f LaserBeam::getEnd() const
{
    return m_end;
}

Vector3f LaserBeam::getStart() const
{
    return m_start;
}
