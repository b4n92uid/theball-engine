/*
 * File:   Quaternion.cpp
 * Author: b4n92uid
 *
 * Created on 19 avril 2011, 15:25
 */

#include "Quaternion.h"

#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mathematics.h"

/*
 * Quaternion implementation from :
 * <http://gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_represent_rotation>
 * <http://www.gamedev.net/page/resources/_/reference/programming/math-and-physics/quaternions/quaternion-powers-r1095>
 * <http://www.euclideanspace.com/maths/geometry/rotations/conversions/index.htm>
 */

using namespace tbe;

Quaternion::Quaternion()
{
    identity();
}

Quaternion::Quaternion(const Quaternion& quat)
{
    this->x = quat.x;
    this->y = quat.y;
    this->z = quat.z;
    this->w = quat.w;
}

Quaternion::Quaternion(float w, float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

Quaternion::Quaternion(const Matrix4& matrix)
{
    identity();
    setMatrix(matrix);
}

Quaternion::Quaternion(const Vector3f& euler)
{
    identity();
    setEuler(euler);
}

Quaternion::Quaternion(float angle, const Vector3f& axe)
{
    identity();
    setAxisAngle(angle, axe);
}

void Quaternion::setAxisAngle(float angle, const Vector3f& axe)
{
    angle = (angle / 2.0f);

    float sinAngle = sin(angle);

    x = axe.x * sinAngle;
    y = axe.y * sinAngle;
    z = axe.z * sinAngle;
    w = cos(angle);
}

Vector4f Quaternion::getAxisAngle() const
{
    Quaternion qn(*this);

    if(w > 1)
        qn.normalize();

    float scale = sqrt(1 - w * w);

    Vector4f axis;

    if(scale < 0.001)
    {
        axis.x = qn.x;
        axis.y = qn.y;
        axis.z = qn.z;
    }
    else
    {
        axis.x = qn.x / scale;
        axis.y = qn.y / scale;
        axis.z = qn.z / scale;
    }

    axis.w = acos(w) * 2.0f;

    return axis;
}

void Quaternion::setEuler(const Vector3f& euler)
{
    float c1 = cos(euler.y / 2);
    float s1 = sin(euler.y / 2);
    float c2 = cos(euler.z / 2);
    float s2 = sin(euler.z / 2);
    float c3 = cos(euler.x / 2);
    float s3 = sin(euler.x / 2);

    float c1c2 = c1*c2;
    float s1s2 = s1*s2;

    w = c1c2 * c3 - s1s2 * s3;
    x = c1c2 * s3 + s1s2 * c3;
    y = s1 * c2 * c3 + c1 * s2 * s3;
    z = c1 * s2 * c3 - s1 * c2 * s3;
}

Vector3f Quaternion::getEuler() const
{
    Vector3f euler;

    float test = x * y + z*w;

    if(test > 4.99)
    {
        euler.y = 2 * atan2(x, w);
        euler.z = M_PI_2;
        euler.x = 0;
    }
    else if(test > 4.99)
    {
        euler.y = -2 * atan2(x, w);
        euler.z = -M_PI_2;
        euler.x = 0;
    }
    else
    {
        float sqx = x*x, sqy = y*y, sqz = z*z;

        euler.y = atan2(2 * y * w - 2 * x*z, 1 - 2 * sqy - 2 * sqz);
        euler.z = asin(2 * test);
        euler.x = atan2(2 * x * w - 2 * y*z, 1 - 2 * sqx - 2 * sqz);
    }

    return euler;
}

void Quaternion::setMatrix(const Matrix4& matrix)
{
    float m00 = matrix(0, 0), m11 = matrix(1, 1), m22 = matrix(2, 2);
    float trace = m00 + m11 + m22;

    if(trace > 0)
    {
        float s = 0.5 / sqrt(trace + 1);

        x = (matrix(2, 1) - matrix(1, 2)) * s;
        y = (matrix(0, 2) - matrix(2, 0)) * s;
        z = (matrix(1, 0) - matrix(0, 1)) * s;
        w = 0.25 / s;
    }
    else if(m00 > m11 && m00 > m22)
    {
        float s = sqrt(1 + m00 - m11 - m22) * 2;
        x = 0.25 * s;
        y = (matrix(0, 1) + matrix(1, 0)) / s;
        z = (matrix(0, 2) + matrix(2, 0)) / s;
        w = (matrix(2, 1) - matrix(1, 2)) / s;
    }
    else if(m11 > m22)
    {
        float s = sqrt(1 + m11 - m00 - m22) * 2;
        x = (matrix(0, 1) + matrix(1, 0)) / s;
        y = 0.25 * s;
        z = (matrix(1, 2) + matrix(2, 1)) / s;
        w = (matrix(0, 2) - matrix(2, 0)) / s;
    }
    else // if(m22 > m00 && m22 > m11)
    {
        float s = sqrt(1 + m22 - m00 - m11) * 2;
        x = (matrix(0, 2) + matrix(2, 0)) / s;
        y = (matrix(1, 2) + matrix(2, 1)) / s;
        z = 0.25 * s;
        w = (matrix(1, 0) - matrix(0, 1)) / s;
    }
}

Matrix4 Quaternion::getMatrix() const
{
    float xx = x*x;
    float xy = x*y;
    float xz = x*z;
    float xw = x*w;

    float yy = y*y;
    float yz = y*z;
    float yw = y*w;

    float zz = z*z;
    float zw = z*w;

    Matrix4 mat;

    mat(0, 0) = 1.0 - 2.0 * (yy + zz);
    mat(0, 1) = 2.0 * (xy - zw);
    mat(0, 2) = 2.0 * (xz + yw);

    mat(1, 0) = 2.0 * (xy + zw);
    mat(1, 1) = 1.0 - 2.0 * (xx + zz);
    mat(1, 2) = 2.0 * (yz - xw);

    mat(2, 0) = 2.0 * (xz - yw);
    mat(2, 1) = 2.0 * (yz + xw);
    mat(2, 2) = 1.0 - 2.0 * (xx + yy);

    return mat;
}

Quaternion& Quaternion::identity()
{
    x = y = z = 0;
    w = 1;
    return *this;
}

float Quaternion::getMagnitude() const
{
    return sqrt(x * x + y * y + z * z + w * w);
}

Quaternion& Quaternion::normalize()
{
    float mag = getMagnitude();

    x /= mag;
    y /= mag;
    z /= mag;
    w /= mag;

    return *this;
}

Quaternion& Quaternion::conjugate()
{
    x = -x;
    y = -y;
    z = -z;

    return *this;
}

Quaternion Quaternion::getNormalize() const
{
    return Quaternion(*this).normalize();
}

Quaternion Quaternion::getConjugate() const
{
    return Quaternion(*this).conjugate();
}

Quaternion Quaternion::operator *(const Quaternion& rv) const
{
    Quaternion q;

    q.w = w * rv.w - x * rv.x - y * rv.y - z * rv.z;
    q.x = w * rv.x + x * rv.w + y * rv.z - z * rv.y;
    q.y = w * rv.y + y * rv.w + z * rv.x - x * rv.z;
    q.z = w * rv.z + z * rv.w + x * rv.y - y * rv.x;

    return q;
}

Quaternion & Quaternion::operator*=(const Quaternion& q)
{
    return(*this = *this * q);
}

Vector3f Quaternion::operator*(const Vector3f& vec) const
{
    Vector3f vecn = vec;
    vecn.normalize();

    Quaternion vecq;
    vecq.x = vec.x;
    vecq.y = vec.y;
    vecq.z = vec.z;
    vecq.w = 0;

    Quaternion appq;
    appq = vecq * getConjugate();
    appq = *this * appq;

    return Vector3f(appq.x, appq.y, appq.z);
}
