/*
 * File:   Frustum.cpp
 * Author: b4n92uid
 *
 * Created on 21 janvier 2010, 21:59
 */

#include "Frustum.h"
#include "Node.h"
#include "Particles.h"

#define PlaneTest(pl, point) (pl.x * point.x + pl.y * point.y + pl.z * point.z + pl.w <= 0)

using namespace tbe;
using namespace tbe::scene;

Frustum::Frustum()
{
}

Frustum::~Frustum()
{
}

bool Frustum::isInside(Node* node)
{
    const AABB& box = node->getAbsolutAabb();
    return isInside(box);
}

bool Frustum::isInside(const AABB& aabb)
{
    Vector3f size(aabb.max.x - aabb.min.x,
                  aabb.max.y - aabb.min.y,
                  aabb.max.z - aabb.min.z);

    Vector3f points[8] = {
        aabb.min,
        aabb.min + Vector3f(size.x, 0, 0),
        aabb.min + Vector3f(0, size.y, 0),
        aabb.min + Vector3f(size.x, size.y, 0),
        aabb.min + Vector3f(0, 0, size.z),
        aabb.min + Vector3f(size.x, 0, size.z),
        aabb.min + Vector3f(0, size.y, size.z),
        aabb.min + size,
    };

    for(unsigned i = 0; i < 6; i++)
    {
        if(!PlaneTest(m_planes[i], points[0])) continue;
        if(!PlaneTest(m_planes[i], points[1])) continue;
        if(!PlaneTest(m_planes[i], points[2])) continue;
        if(!PlaneTest(m_planes[i], points[3])) continue;
        if(!PlaneTest(m_planes[i], points[4])) continue;
        if(!PlaneTest(m_planes[i], points[5])) continue;
        if(!PlaneTest(m_planes[i], points[6])) continue;
        if(!PlaneTest(m_planes[i], points[7])) continue;

        return false;
    }

    return true;
}

bool Frustum::isInside(Vector3f point)
{
    for(unsigned i = 0; i < 6; i++)
        if(PlaneTest(m_planes[i], point))
            return false;

    return true;
}

#ifdef FAR
#undef FAR
#endif

#ifdef NEAR
#undef NEAR
#endif

void Frustum::extractPlane()
{

    enum Sides
    {
        RIGHT, LEFT, BOTTOM, TOP, FAR, NEAR,
    };

    Matrix4f modelview, projection, clip;

    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
    glGetFloatv(GL_PROJECTION_MATRIX, projection);

    clip = modelview * projection;

    m_planes[RIGHT] = Vector4f(clip[ 3] - clip[ 0], clip[ 7] - clip[ 4], clip[11] - clip[ 8], clip[15] - clip[12]).normalize();
    m_planes[LEFT] = Vector4f(clip[ 3] + clip[ 0], clip[ 7] + clip[ 4], clip[11] + clip[ 8], clip[15] + clip[12]).normalize();
    m_planes[BOTTOM] = Vector4f(clip[ 3] + clip[ 1], clip[ 7] + clip[ 5], clip[11] + clip[ 9], clip[15] + clip[13]).normalize();
    m_planes[TOP] = Vector4f(clip[ 3] - clip[ 1], clip[ 7] - clip[ 5], clip[11] - clip[ 9], clip[15] - clip[13]).normalize();
    m_planes[FAR] = Vector4f(clip[ 3] - clip[ 2], clip[ 7] - clip[ 6], clip[11] - clip[10], clip[15] - clip[14]).normalize();
    m_planes[NEAR] = Vector4f(clip[ 3] + clip[ 2], clip[ 7] + clip[ 6], clip[11] + clip[10], clip[15] + clip[14]).normalize();
}
