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

Quaternion::Quaternion(float x, float y, float z, float w)
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

Quaternion::Quaternion(const Vector3f& rotation)
{
    identity();
    setEuler(rotation);
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

void Quaternion::setEuler(const Vector3f& rotation)
{
    Vector3f euler = rotation;

    float h = euler.y;
    float a = euler.z;
    float b = euler.x;

    float c1 = cos(h), c2 = cos(a), c3 = cos(b);
    float s1 = sin(h), s2 = sin(a), s3 = sin(b);

    w = sqrt(1 + c1 * c2 + c1 * c3 - s1 * s2 * s3 + c2 * c3) / 2.0;

    double w4 = (4.0 * w);

    x = (c2 * s3 + c1 * s3 + s1 * s2 * c3) / w4;
    y = (s1 * c2 + s1 * c3 + c1 * s2 * s3) / w4;
    z = (-s1 * s3 + c1 * s2 * c3 + s2) / w4;
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
        float s = sqrt(trace + 1)*2;

        x = (matrix(2, 1) - matrix(1, 2)) / s;
        y = (matrix(0, 2) - matrix(2, 0)) / s;
        z = (matrix(1, 0) - matrix(0, 1)) / s;
        w = 0.25 * s;
    }
    else if(m00 > m11 && m00 > m22)
    {
        float s = sqrt(1 + m00 - m11 - m22) * 2;
        x = 0.25 * s;
        y = (matrix(0, 1) - matrix(1, 0)) / s;
        z = (matrix(0, 2) - matrix(2, 0)) / s;
        w = (matrix(2, 1) - matrix(1, 2)) / s;
    }
    else if(m11 > m00 && m11 > m22)
    {
        float s = sqrt(1 + m11 - m00 - m22) * 2;
        x = (matrix(0, 1) - matrix(1, 0)) / s;
        y = 0.25 * s;
        z = (matrix(1, 2) - matrix(2, 1)) / s;
        w = (matrix(0, 2) - matrix(2, 0)) / s;
    }
    else // if(m22 > m00 && m22 > m11)
    {
        float s = sqrt(1 + m22 - m00 - m11) * 2;
        x = (matrix(0, 2) - matrix(2, 0)) / s;
        y = (matrix(1, 2) - matrix(2, 1)) / s;
        z = 0.25 * s;
        w = (matrix(1, 0) - matrix(0, 1)) / s;
    }
}

Matrix4 Quaternion::getMatrix() const
{
    float x2 = x*x;
    float xy = x*y;
    float xz = x*z;

    float y2 = y*y;
    float yz = y*z;

    float z2 = z*z;

    float wx = w * x;
    float wy = w * y;
    float wz = w * z;

    Matrix4 mat;

    mat(0, 0) = 1.0 - 2.0 * (y2 + z2);
    mat(0, 1) = 2.0 * (xy - wz);
    mat(0, 2) = 2.0 * (xz + wy);
    mat(0, 3) = 0.0;

    mat(1, 0) = 2.0 * (xy + wz);
    mat(1, 1) = 1.0 - 2.0 * (x2 + z2);
    mat(1, 2) = 2.0 * (yz - wx);
    mat(1, 3) = 0.0;

    mat(2, 0) = 2.0 * (xz - wy);
    mat(2, 1) = 2.0 * (yz + wx);
    mat(2, 2) = 1.0 - 2.0 * (x2 + y2);
    mat(2, 3) = 0.0;

    mat(3, 0) = 0.0;
    mat(3, 1) = 0.0;
    mat(3, 2) = 0.0;
    mat(3, 3) = 1.0;

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

    q.x = x * rv.w + y * rv.z + z * rv.y - w * rv.x;
    q.y = -x * rv.z + y * rv.w + z * rv.x - w * rv.y;
    q.z = x * rv.y - y * rv.x + z * rv.w - w * rv.z;
    q.w = -x * rv.x - y * rv.y - z * rv.z - w * rv.w;

    return q;
}

Quaternion & Quaternion::operator*=(const Quaternion& q)
{
    return (*this = q * * this);
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
