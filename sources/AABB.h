/*
 * File:   AABB.h
 * Author: b4n92uid
 *
 * Created on 24 novembre 2009, 18:53
 */

#ifndef _AABB_H
#define	_AABB_H

#include "Mathematics.h"
#include "HardwareBuffer.h"

namespace tbe
{

namespace scene
{
class Node;
}

/// \brief Axis Aligned Bounding Box

class AABB
{
public:
    AABB();
    AABB(float minmax);
    AABB(Vector3f min, Vector3f max);
    virtual ~AABB();

    AABB& count(scene::Node* node);
    AABB& count(const AABB& aabb);
    AABB& count(const Vector3f& pos);

    AABB & operator()(const Vector3f& min, const Vector3f& max);

    AABB & add(const Vector3f& pos);
    AABB & add(const AABB& aabb);

    AABB & sub(const Vector3f& pos);
    AABB & sub(const AABB& aabb);

    AABB & operator+=(const Vector3f& pos);
    AABB & operator+=(const AABB& aabb);

    AABB & operator-=(const Vector3f& pos);
    AABB & operator-=(const AABB& aabb);

    AABB operator+(const Vector3f& value);
    AABB operator+(const AABB& aabb);

    AABB operator-(const Vector3f& value);
    AABB operator-(const AABB& aabb);

    bool isInner(scene::Node* node) const;
    bool isInner(const AABB& aabb) const;
    bool isInner(const Vector3f& point) const;

    void clear();

    float getSize() const;
    Vector3f getCenter() const;

    Vector3f min;
    Vector3f max;
};

}

#endif	/* _AABB_H */

