/*
 * File:   Matrix4.cpp
 * Author: b4n92uid
 *
 * Created on 19 avril 2011, 15:18
 */

#include <GL/gl.h>

#include "Matrix4.h"

#include "Quaternion.h"
#include "Vector3.h"

using namespace tbe;

Matrix4::Matrix4()
{
    identity();
}

Matrix4::Matrix4(const float fmatrix[16])
{
    for(unsigned i = 0; i < 16; i++)
        m_matrix[i] = fmatrix[i];
}

Matrix4::Matrix4(const Vector3f& pos)
{
    identity();

    setPos(pos);
}

Matrix4::Matrix4(std::string exp)
{
    identity();

    char sep;
    std::stringstream ss(exp);

    for(unsigned i = 0; i < 15; i++)
        ss >> m_matrix[i] >> sep;

    ss >> m_matrix[15];
}

Matrix4::Matrix4(const Matrix4& copy)
{
    *this = copy;
}

void Matrix4::identity()
{
    for(unsigned i = 0; i < 16; i++)
        m_matrix[i] = 0.0f;

    m_matrix[0] = m_matrix[5] = m_matrix[10] = m_matrix[15] = 1.0f;
}

void Matrix4::transpose()
{
    float t;

    t = m_matrix[ 1];
    m_matrix[ 1] = m_matrix[ 4];
    m_matrix[ 4] = t;

    t = m_matrix[ 2];
    m_matrix[ 2] = m_matrix[ 8];
    m_matrix[ 8] = t;

    t = m_matrix[ 3];
    m_matrix[ 3] = m_matrix[12];
    m_matrix[12] = t;

    t = m_matrix[ 6];
    m_matrix[ 6] = m_matrix[ 9];
    m_matrix[ 9] = t;

    t = m_matrix[ 7];
    m_matrix[ 7] = m_matrix[13];
    m_matrix[13] = t;

    t = m_matrix[11];
    m_matrix[11] = m_matrix[14];
    m_matrix[14] = t;
}

bool Matrix4::operator=(const Matrix4& copy)
{
    for(unsigned i = 0; i < 16; i++)
        m_matrix[i] = copy.m_matrix[i];

    return true;
}

void Matrix4::setPos(Vector3f value)
{
    m_matrix[12] = value.x;
    m_matrix[13] = value.y;
    m_matrix[14] = value.z;
}

Vector3f Matrix4::getPos() const
{
    return Vector3<float > (m_matrix[12], m_matrix[13], m_matrix[14]);
}

void Matrix4::setScale(Vector3f value)
{
    m_matrix[0] = value.x;
    m_matrix[5] = value.y;
    m_matrix[10] = value.z;
}

Vector3f Matrix4::getScale() const
{
    return Vector3<float > (m_matrix[0], m_matrix[5], m_matrix[10]);
}

void Matrix4::setRotate(Quaternion rotation)
{
    const Matrix4& apply = rotation.getMatrix();

    m_matrix[ 0] = apply[ 0];
    m_matrix[ 1] = apply[ 1];
    m_matrix[ 2] = apply[ 2];

    m_matrix[ 4] = apply[ 4];
    m_matrix[ 5] = apply[ 5];
    m_matrix[ 6] = apply[ 6];

    m_matrix[ 8] = apply[ 8];
    m_matrix[ 9] = apply[ 9];
    m_matrix[10] = apply[10];
}

void Matrix4::setRotate(float angle, Vector3f axe)
{
    float c = cos(angle);
    float s = sin(angle);

    float &x = axe.x, &y = axe.y, &z = axe.z;

    m_matrix[ 0] = x * x * (1 - c) + c;
    m_matrix[ 1] = y * x * (1 - c) + z * s;
    m_matrix[ 2] = x * z * (1 - c) - y * s;

    m_matrix[ 4] = x * y * (1 - c) - z * s;
    m_matrix[ 5] = y * y * (1 - c) + c;
    m_matrix[ 6] = y * z * (1 - c) + x * s;

    m_matrix[ 8] = x * z * (1 - c) + y * s;
    m_matrix[ 9] = y * z * (1 - c) - x * s;
    m_matrix[10] = z * z * (1 - c) + c;
}

Quaternion Matrix4::getRotate() const
{
    Quaternion quat;
    quat.setMatrix(*this);

    return quat;
}

void Matrix4::setRotateX(float v)
{
    float cosinus = cos(v);
    float sinus = sin(v);

    Matrix4& mat = *this;
    mat[ 5] = cosinus;
    mat[ 6] = -sinus;
    mat[ 9] = sinus;
    mat[10] = cosinus;
}

void Matrix4::setRotateY(float v)
{
    float cosinus = cos(v);
    float sinus = sin(v);

    Matrix4& mat = *this;
    mat[ 0] = cosinus;
    mat[ 2] = -sinus;
    mat[ 8] = sinus;
    mat[10] = cosinus;
}

void Matrix4::setRotateZ(float v)
{
    float cosinus = cos(v);
    float sinus = sin(v);

    Matrix4& mat = *this;
    mat[0] = cosinus;
    mat[1] = -sinus;
    mat[4] = sinus;
    mat[5] = cosinus;
}

void Matrix4::setRotate(Vector3f v)
{
    setRotateX(v.x);
    setRotateY(v.y);
    setRotateZ(v.z);
}

Matrix4::operator Vector3f()
{
    return getPos();
}

Matrix4::operator const float*() const
{
    return m_matrix;
}

Matrix4::operator float*()
{
    return m_matrix;
}

float Matrix4::operator()(int i, int j)const
{
    return m_matrix[i * 4 + j];
}

float Matrix4::operator[](int i)const
{
    return m_matrix[i];
}

float & Matrix4::operator()(int i, int j)
{
    return m_matrix[i * 4 + j];
}

float & Matrix4::operator[](int i)
{
    return m_matrix[i];
}

Matrix4 Matrix4::operator*(const Matrix4& mat) const
{
    Matrix4 ret;

    ret.m_matrix[ 0] = (m_matrix[ 0] * mat.m_matrix[ 0]) + (m_matrix[ 1] * mat.m_matrix[ 4]) + (m_matrix[ 2] * mat.m_matrix[ 8]) + (m_matrix[ 3] * mat.m_matrix[12]);
    ret.m_matrix[ 1] = (m_matrix[ 0] * mat.m_matrix[ 1]) + (m_matrix[ 1] * mat.m_matrix[ 5]) + (m_matrix[ 2] * mat.m_matrix[ 9]) + (m_matrix[ 3] * mat.m_matrix[13]);
    ret.m_matrix[ 2] = (m_matrix[ 0] * mat.m_matrix[ 2]) + (m_matrix[ 1] * mat.m_matrix[ 6]) + (m_matrix[ 2] * mat.m_matrix[10]) + (m_matrix[ 3] * mat.m_matrix[14]);
    ret.m_matrix[ 3] = (m_matrix[ 0] * mat.m_matrix[ 3]) + (m_matrix[ 1] * mat.m_matrix[ 7]) + (m_matrix[ 2] * mat.m_matrix[11]) + (m_matrix[ 3] * mat.m_matrix[15]);

    ret.m_matrix[ 4] = (m_matrix[ 4] * mat.m_matrix[ 0]) + (m_matrix[ 5] * mat.m_matrix[ 4]) + (m_matrix[ 6] * mat.m_matrix[ 8]) + (m_matrix[ 7] * mat.m_matrix[12]);
    ret.m_matrix[ 5] = (m_matrix[ 4] * mat.m_matrix[ 1]) + (m_matrix[ 5] * mat.m_matrix[ 5]) + (m_matrix[ 6] * mat.m_matrix[ 9]) + (m_matrix[ 7] * mat.m_matrix[13]);
    ret.m_matrix[ 6] = (m_matrix[ 4] * mat.m_matrix[ 2]) + (m_matrix[ 5] * mat.m_matrix[ 6]) + (m_matrix[ 6] * mat.m_matrix[10]) + (m_matrix[ 7] * mat.m_matrix[14]);
    ret.m_matrix[ 7] = (m_matrix[ 4] * mat.m_matrix[ 3]) + (m_matrix[ 5] * mat.m_matrix[ 7]) + (m_matrix[ 6] * mat.m_matrix[11]) + (m_matrix[ 7] * mat.m_matrix[15]);

    ret.m_matrix[ 8] = (m_matrix[ 8] * mat.m_matrix[ 0]) + (m_matrix[ 9] * mat.m_matrix[ 4]) + (m_matrix[10] * mat.m_matrix[ 8]) + (m_matrix[11] * mat.m_matrix[12]);
    ret.m_matrix[ 9] = (m_matrix[ 8] * mat.m_matrix[ 1]) + (m_matrix[ 9] * mat.m_matrix[ 5]) + (m_matrix[10] * mat.m_matrix[ 9]) + (m_matrix[11] * mat.m_matrix[13]);
    ret.m_matrix[10] = (m_matrix[ 8] * mat.m_matrix[ 2]) + (m_matrix[ 9] * mat.m_matrix[ 6]) + (m_matrix[10] * mat.m_matrix[10]) + (m_matrix[11] * mat.m_matrix[14]);
    ret.m_matrix[11] = (m_matrix[ 8] * mat.m_matrix[ 3]) + (m_matrix[ 9] * mat.m_matrix[ 7]) + (m_matrix[10] * mat.m_matrix[11]) + (m_matrix[11] * mat.m_matrix[15]);

    ret.m_matrix[12] = (m_matrix[12] * mat.m_matrix[ 0]) + (m_matrix[13] * mat.m_matrix[ 4]) + (m_matrix[14] * mat.m_matrix[ 8]) + (m_matrix[15] * mat.m_matrix[12]);
    ret.m_matrix[13] = (m_matrix[12] * mat.m_matrix[ 1]) + (m_matrix[13] * mat.m_matrix[ 5]) + (m_matrix[14] * mat.m_matrix[ 9]) + (m_matrix[15] * mat.m_matrix[13]);
    ret.m_matrix[14] = (m_matrix[12] * mat.m_matrix[ 2]) + (m_matrix[13] * mat.m_matrix[ 6]) + (m_matrix[14] * mat.m_matrix[10]) + (m_matrix[15] * mat.m_matrix[14]);
    ret.m_matrix[15] = (m_matrix[12] * mat.m_matrix[ 3]) + (m_matrix[13] * mat.m_matrix[ 7]) + (m_matrix[14] * mat.m_matrix[11]) + (m_matrix[15] * mat.m_matrix[15]);

    return ret;
}

Matrix4 & Matrix4::operator*=(const Matrix4& mat)
{
    *this = *this * mat;
    return *this;
}

Vector3f Matrix4::operator*(const Vector3f& vec) const
{
    return Vector3<float > (
            m_matrix[0] * vec[0] + m_matrix[4] * vec[1] + m_matrix[8] * vec[2] + m_matrix[12],
            m_matrix[1] * vec[0] + m_matrix[5] * vec[1] + m_matrix[9] * vec[2] + m_matrix[13],
            m_matrix[2] * vec[0] + m_matrix[6] * vec[1] + m_matrix[10] * vec[2] + m_matrix[14]
            );
}

Vector3f Matrix4::operator*=(const Vector3f& vec)
{
    *this = *this * vec;
    return *this;
}

void Matrix4::translate(Vector3f pos)
{
    m_matrix[12] += pos.x;
    m_matrix[13] += pos.y;
    m_matrix[14] += pos.z;
}

void Matrix4::rotate(Vector3f euler)
{
    Matrix4 mat;
    mat.setRotate(euler);

    *this *= mat;
}

void Matrix4::rotate(float angle, Vector3f axe)
{
    Matrix4 mat;
    mat.setRotate(angle, axe);

    *this *= mat;
}

void Matrix4::rotate(Quaternion rotation)
{
    *this *= rotation.getMatrix();
}

void Matrix4::scale(Vector3f scale)
{
    //    Matrix4 scaleMat(0, scale);
    //     *this *= scaleMat;

    m_matrix[ 0] += scale.x;
    m_matrix[ 5] += scale.y;
    m_matrix[10] += scale.z;
}

Matrix4 Matrix4::translate(Matrix4 mat, Vector3f pos)
{
    mat.translate(pos);
    return mat;
}

Matrix4 Matrix4::rotate(Matrix4 mat, float angle, Vector3f axe)
{
    mat.rotate(angle, axe);
    return mat;
}

Matrix4 Matrix4::rotate(Matrix4 mat, Quaternion quat)
{
    mat.rotate(quat);
    return mat;
}

Matrix4 Matrix4::scale(Matrix4 mat, Vector3f pos)
{
    mat.scale(pos);
    return mat;
}

Matrix4 Matrix4::transpose(Matrix4 mat)
{
    Matrix4 ini = mat;
    ini.transpose();

    return ini;
}
