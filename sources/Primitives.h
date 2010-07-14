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

class Node;

/**
 * \brief Primitive de type cube
 */
class Box : public Mesh
{
public:
    Box();
    Box(Vector3f size);

    void Setup(Vector3f size);

    Vector3f GetSize() const;

protected:
    Vector3f m_size;

};

/**
 * \brief Primitive de type Sphere
 */
class Sphere : public Mesh
{
public:
    Sphere();
    Sphere(float radius, unsigned slices, unsigned stackes);

    void Setup(float radius, unsigned slices, unsigned stackes);

    float GetRadius() const;

protected:
    float m_radius;
};

/**
 * \brief Axes aligner en X, Y, Z
 */
class Axes : public Mesh
{
public:
    Axes();
    Axes(float lineWidth, float lineLength);

    void Setup(float lineWidth, float lineLength);

    float GetLineLength() const;
    float GetLineWidth() const;

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
    Plane();
    Plane(Vector2f size, Vector2i cut);

    void Setup(Vector2f size, Vector2i cut);

    Vector2i GetCut() const;
    Vector2f GetSize() const;

protected:
    Vector2f m_size;
    Vector2i m_cut;
};



}
}

#endif	/* _PRIMITIVES_H */

