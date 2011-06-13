/*
 * File:   Primitives.h
 * Author: b4n92uid
 *
 * Created on 24 novembre 2009, 17:34
 */

#ifndef _PRIMITIVES_H
#define	_PRIMITIVES_H

#include "Mesh.h"

namespace tbe
{
namespace scene
{

class MeshParallelScene;

/**
 * \brief Primitive de type cube
 */
class Box : public Mesh
{
public:
    Box(MeshParallelScene* scene);
    Box(MeshParallelScene* scene, Vector3f size);

    void setup(Vector3f size);

    void setSize(Vector3f size);
    Vector3f getSize() const;

protected:
    Vector3f m_size;

};

/**
 * \brief Primitive de type Sphere
 */
class Sphere : public Mesh
{
public:
    Sphere(MeshParallelScene* scene);
    Sphere(MeshParallelScene* scene, float radius, unsigned slices, unsigned stackes);

    void setup(float radius, unsigned slices, unsigned stackes);

    float getRadius() const;

protected:
    float m_radius;
};

/**
 * \brief Axes aligner en X, Y, Z
 */
class Axes : public Mesh
{
public:
    Axes(MeshParallelScene* scene);
    Axes(MeshParallelScene* scene, float lineWidth, float lineLength);

    void setup(float lineWidth, float lineLength);

    float getLineLength() const;
    float getLineWidth() const;

protected:
    float m_lineWidth;
    float m_lineLength;

};

/**
 * \brief Primitives de type Plan
 */
class Plane : public Mesh
{
public:
    Plane(MeshParallelScene* scene);
    Plane(MeshParallelScene* scene, Vector2f size, Vector2i cut);

    void setup(Vector2f size, Vector2i cut);

    Vector2i getCut() const;
    Vector2f getSize() const;

protected:
    Vector2f m_size;
    Vector2i m_cut;
};


/**
 * \brief Primitives de type Grille
 */
class Grid : public Mesh
{
public:
    Grid(MeshParallelScene* scene);
    Grid(MeshParallelScene* scene, Vector2f size, Vector2i cut);

    void setup(Vector2f size, Vector2i cut);

    Vector2i getCut() const;
    Vector2f getSize() const;

protected:
    Vector2f m_size;
    Vector2i m_cut;
};



}
}

#endif	/* _PRIMITIVES_H */

