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
    AABB(Vector3f min, Vector3f max);
    virtual ~AABB();

    AABB& Count(scene::Node* node);
    AABB& Count(const AABB& aabb);
    AABB& Count(const Vector3f& pos);

    AABB& Add(const AABB& aabb);
    AABB& Add(const Vector3f& pos);

    bool IsInner(scene::Node* node) const;
    bool IsInner(const AABB& aabb) const;
    bool IsInner(const Vector3f& point) const;

    void Clear();

    float GetSize() const;

    Vector3f min;
    Vector3f max;
};

}

#endif	/* _AABB_H */

