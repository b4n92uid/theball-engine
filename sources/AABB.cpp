/*
 * File:   AABB.cpp
 * Author: b4n92uid
 *
 * Created on 24 novembre 2009, 18:53
 */

#include "AABB.h"

#include "Node.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

AABB::AABB()
{
}

AABB::AABB(Vector3f min, Vector3f max)
{
    this->min = min;
    this->max = max;
}

AABB::~AABB()
{
}

void AABB::Clear()
{
    min = max = 0;
}

AABB& AABB::Count(scene::Node* node)
{
    Count(node->GetAbsolutAabb());

    return *this;
}

AABB& AABB::Count(const AABB& aabb)
{
    Count(aabb.min);
    Count(aabb.max);

    return *this;
}

AABB& AABB::Count(const Vector3f& pos)
{
    if(min.x > pos.x) min.x = pos.x;
    if(min.y > pos.y) min.y = pos.y;
    if(min.z > pos.z) min.z = pos.z;

    if(max.x < pos.x) max.x = pos.x;
    if(max.y < pos.y) max.y = pos.y;
    if(max.z < pos.z) max.z = pos.z;

    return *this;
}

AABB& AABB::Add(const AABB& aabb)
{
    min += aabb.min;
    max += aabb.max;

    return *this;
}

AABB& AABB::Add(const Vector3f& pos)
{
    min -= pos;
    max += pos;

    return *this;
}

bool AABB::IsInner(scene::Node* node) const
{
    return IsInner(node->GetPos());
}

bool AABB::IsInner(const AABB& aabb) const
{
    return(aabb.min.IsInside(min, max) || aabb.max.IsInside(min, max));
}

bool AABB::IsInner(const Vector3f& point) const
{
    return point.IsInside(min, max);
}

float AABB::GetSize() const
{
    return(max - min).GetMagnitude();
}
