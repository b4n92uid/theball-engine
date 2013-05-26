/*
 * File:   Frustum.h
 * Author: b4n92uid
 *
 * Created on 21 janvier 2010, 21:59
 */

#ifndef _FRUSTUM_H
#define	_FRUSTUM_H

#include "AABB.h"
#include "Mathematics.h"

namespace tbe
{
namespace scene
{

class Node;
class ParticlesEmiter;

/**
 * \brief Représentation du frustum culling
 * inclus des fonction de test
 */
class Frustum
{
public:
    Frustum();
    virtual ~Frustum();

    void extractPlane();

    bool isInside(Node* node) const;
    bool isInside(const AABB& aabb) const;
    bool isInside(Vector3f point) const;
    
private:
    Vector4f m_planes[6];
};

}
}

#endif	/* _FRUSTUM_H */

