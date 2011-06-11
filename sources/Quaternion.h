/*
 * File:   Quaternion.h
 * Author: b4n92uid
 *
 * Created on 19 avril 2011, 15:25
 *
 * http://gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_represent_rotation
 * http://www.gamedev.net/page/resources/_/reference/programming/math-and-physics/quaternions/quaternion-powers-r1095
 * http://www.euclideanspace.com/maths/geometry/rotations/conversions/index.htm
 */

#ifndef QUATERNION_H
#define	QUATERNION_H

#include <iostream>

namespace tbe
{

template<typename T> class Vector3;
template<typename T> class Vector4;

typedef Vector3<float> Vector3f;
typedef Vector4<float> Vector4f;

class Matrix4;

class Quaternion
{
public:
    Quaternion();
    Quaternion(const Quaternion& quat);
    Quaternion(float x, float y, float z, float w);
    Quaternion(const Matrix4& matrix);
    Quaternion(const Vector3f& euler);
    Quaternion(float angle, const Vector3f& axe);

    void setAxisAngle(float angle, const Vector3f& axe);
    Vector4f getAxisAngle() const;

    void setEuler(const Vector3f& euler);
    Vector3f getEuler() const;

    void setMatrix(const Matrix4& matrix);
    Matrix4 getMatrix() const;

    Quaternion& identity();

    Quaternion& normalize();
    Quaternion& conjugate();

    float getMagnitude() const;

    Quaternion getNormalize() const;
    Quaternion getConjugate() const;

    Quaternion operator*(const Quaternion& rv) const;
    Vector3f operator*(const Vector3f& vec) const;

    Quaternion & operator*=(const Quaternion& quat);

    friend std::ostream & operator<<(std::ostream& out, Quaternion& quat)
    {
        return out << quat.x << ", " << quat.y << ", " << quat.z << ", " << quat.w;
    }

    float x, y, z, w;
};

}

#endif	/* QUATERNION_H */

