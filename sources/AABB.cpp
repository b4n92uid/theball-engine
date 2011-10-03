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

AABB::AABB(float minmax)
{
    this->max = minmax;
    this->min = -minmax;
}

AABB::AABB(Vector3f min, Vector3f max)
{
    this->min = min;
    this->max = max;
}

AABB::~AABB()
{
}

void AABB::clear()
{
    min = max = 0;
}

AABB& AABB::count(scene::Node* node)
{
    count(node->getAbsolutAabb());

    return *this;
}

AABB& AABB::count(const AABB& aabb)
{
    count(aabb.min);
    count(aabb.max);

    return *this;
}

AABB& AABB::count(const Vector3f& pos)
{
    if(min.x > pos.x) min.x = pos.x;
    if(min.y > pos.y) min.y = pos.y;
    if(min.z > pos.z) min.z = pos.z;

    if(max.x < pos.x) max.x = pos.x;
    if(max.y < pos.y) max.y = pos.y;
    if(max.z < pos.z) max.z = pos.z;

    return *this;
}

bool AABB::isInner(scene::Node* node) const
{
    return isInner(node->getPos());
}

bool AABB::isInner(const AABB& aabb) const
{
    return (aabb.min.isInside(min, max) || aabb.max.isInside(min, max));
}

bool AABB::isInner(const Vector3f& point) const
{
    return point.isInside(min, max);
}

Vector3f AABB::getCenter() const
{
    return (max - min) / 2.0f + min;
}

Vector3f AABB::getSize() const
{
    return (max - min);
}

float AABB::getLength() const
{
    return (max - min).getMagnitude();
}

AABB& AABB::operator()(const Vector3f& min, const Vector3f& max)
{
    this->min = min>this->max? max : min;
    this->max = max<this->min? min : max;

    return *this;
}

AABB & AABB::add(float pos)
{
    return *this += pos;
}

AABB & AABB::add(const Vector3f& pos)
{
    return *this += pos;
}

AABB & AABB::add(const AABB& aabb)
{
    return *this += aabb;
}

AABB & AABB::sub(const Vector3f& pos)
{
    return *this -= pos;
}

AABB & AABB::sub(const AABB& aabb)
{
    return *this -= aabb;
}

AABB& AABB::operator+=(float pos)
{
    return (*this)(min - pos, max + pos);
}

AABB& AABB::operator+=(const Vector3f& pos)
{
    return (*this)(min - pos, max + pos);
}

AABB& AABB::operator+=(const AABB& aabb)
{
    return (*this)(min + aabb.min, max + aabb.max);
}

AABB& AABB::operator-=(const Vector3f& pos)
{
    return (*this)(min + pos, max - pos);
}

AABB& AABB::operator-=(const AABB& aabb)
{
    return (*this)(min + aabb.min, max - aabb.max);
}

AABB AABB::operator-(const Vector3f& value)
{
    return AABB(*this) -= value;
}

AABB AABB::operator-(const AABB& aabb)
{
    return AABB(*this) -= aabb;
}

AABB AABB::operator+(const Vector3f& value)
{
    return AABB(*this) += value;
}

AABB AABB::operator+(const AABB& aabb)
{
    return AABB(*this) += aabb;
}

Vector3f AABB::randPos() const
{
    return Vector3f(math::rand(min.x, max.x),
                    math::rand(min.z, max.y),
                    math::rand(min.y, max.z));
}

AABB& AABB::translate(Vector3f pos)
{
    min += pos;
    max += pos;

    return *this;
}
