/*
 * File:   Matrix4.cpp
 * Author: b4n92uid
 *
 * Created on 19 avril 2011, 15:18
 */

#include "Matrix4.h"

#include "Mathematics.h"
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
        values[i] = fmatrix[i];
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
        ss >> values[i] >> sep;

    ss >> values[15];
}

Matrix4::Matrix4(const Matrix4& copy)
{
    *this = copy;
}

Matrix4::Matrix4(float r1, float r5, float r9, float r13,
                 float r2, float r6, float r10, float r14,
                 float r3, float r7, float r11, float r15,
                 float r4, float r8, float r12, float r16)
{
    values[ 0] = r1;
    values[ 1] = r2;
    values[ 2] = r3;
    values[ 3] = r4;
    values[ 4] = r5;
    values[ 5] = r6;
    values[ 6] = r7;
    values[ 7] = r8;
    values[ 8] = r9;
    values[ 9] = r10;
    values[10] = r11;
    values[11] = r12;
    values[12] = r13;
    values[13] = r14;
    values[14] = r15;
    values[15] = r16;
}

void Matrix4::identity()
{
    for(unsigned i = 0; i < 16; i++)
        values[i] = 0.0f;

    values[0] = values[5] = values[10] = values[15] = 1.0f;
}

void Matrix4::transpose()
{
    float t;

    t = values[ 1];
    values[ 1] = values[ 4];
    values[ 4] = t;

    t = values[ 2];
    values[ 2] = values[ 8];
    values[ 8] = t;

    t = values[ 3];
    values[ 3] = values[12];
    values[12] = t;

    t = values[ 6];
    values[ 6] = values[ 9];
    values[ 9] = t;

    t = values[ 7];
    values[ 7] = values[13];
    values[13] = t;

    t = values[11];
    values[11] = values[14];
    values[14] = t;
}

bool Matrix4::operator=(const Matrix4& copy)
{
    for(unsigned i = 0; i < 16; i++)
        values[i] = copy.values[i];

    return true;
}

void Matrix4::setPos(Vector3f value)
{
    values[12] = value.x;
    values[13] = value.y;
    values[14] = value.z;
}

Vector3f Matrix4::getPos() const
{
    return Vector3f(values[12], values[13], values[14]);
}

Matrix4& Matrix4::translate(const Vector3f& pos)
{
    values[12] += pos.x;
    values[13] += pos.y;
    values[14] += pos.z;

    return *this;
}

Matrix4& Matrix4::rotate(const Quaternion& rotation)
{
    *this *= rotation.getMatrix();

    return *this;
}

Matrix4& Matrix4::scale(const Vector3f& scale)
{
    values[ 0] *= scale.x;
    values[ 1] *= scale.x;
    values[ 2] *= scale.x;
    values[ 4] *= scale.y;
    values[ 5] *= scale.y;
    values[ 6] *= scale.y;
    values[ 8] *= scale.z;
    values[ 9] *= scale.z;
    values[10] *= scale.z;

    return *this;
}

void Matrix4::transform(Vector3f position, Quaternion rotation, Vector3f scale)
{
    Matrix4 transform_mat = rotation.getMatrix();

    transform_mat[ 0] *= scale.x;
    transform_mat[ 1] *= scale.x;
    transform_mat[ 2] *= scale.x;
    transform_mat[ 4] *= scale.y;
    transform_mat[ 5] *= scale.y;
    transform_mat[ 6] *= scale.y;
    transform_mat[ 8] *= scale.z;
    transform_mat[ 9] *= scale.z;
    transform_mat[10] *= scale.z;

    transform_mat[12] = position.x;
    transform_mat[13] = position.y;
    transform_mat[14] = position.z;

    *this *= transform_mat;
}

void qduDecomposition(float m[3][3], float kQ[3][3], Vector3f& kD, Vector3f& kU)
{
    /*
     * QR decomposition :
     * Implemented from Ogre SDK Matrix3::QDUDecomposition
     * 
     * This algorithm uses Gram-Schmidt orthogonalization (the QR algorithm).
     * 
     * <https://bitbucket.org/sinbad/ogre/src/4aa2ca2384f4/OgreMain/src/OgreMatrix3.cpp>
     */

    float fInvLength = math::reverseSqrt(m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0]);
    kQ[0][0] = m[0][0] * fInvLength;
    kQ[1][0] = m[1][0] * fInvLength;
    kQ[2][0] = m[2][0] * fInvLength;

    float fDot = kQ[0][0] * m[0][1] + kQ[1][0] * m[1][1] + kQ[2][0] * m[2][1];
    kQ[0][1] = m[0][1] - fDot * kQ[0][0];
    kQ[1][1] = m[1][1] - fDot * kQ[1][0];
    kQ[2][1] = m[2][1] - fDot * kQ[2][0];

    fInvLength = math::reverseSqrt(kQ[0][1] * kQ[0][1] + kQ[1][1] * kQ[1][1] + kQ[2][1] * kQ[2][1]);
    kQ[0][1] *= fInvLength;
    kQ[1][1] *= fInvLength;
    kQ[2][1] *= fInvLength;

    fDot = kQ[0][0] * m[0][2] + kQ[1][0] * m[1][2] + kQ[2][0] * m[2][2];
    kQ[0][2] = m[0][2] - fDot * kQ[0][0];
    kQ[1][2] = m[1][2] - fDot * kQ[1][0];
    kQ[2][2] = m[2][2] - fDot * kQ[2][0];

    fDot = kQ[0][1] * m[0][2] + kQ[1][1] * m[1][2] + kQ[2][1] * m[2][2];
    kQ[0][2] -= fDot * kQ[0][1];
    kQ[1][2] -= fDot * kQ[1][1];
    kQ[2][2] -= fDot * kQ[2][1];

    fInvLength = math::reverseSqrt(kQ[0][2] * kQ[0][2] + kQ[1][2] * kQ[1][2] + kQ[2][2] * kQ[2][2]);
    kQ[0][2] *= fInvLength;
    kQ[1][2] *= fInvLength;
    kQ[2][2] *= fInvLength;

    // guarantee that orthogonal matrix has determinant 1 (no reflections)
    float fDet =
            kQ[0][0] * kQ[1][1] * kQ[2][2] + kQ[0][1] * kQ[1][2] * kQ[2][0] +
            kQ[0][2] * kQ[1][0] * kQ[2][1] - kQ[0][2] * kQ[1][1] * kQ[2][0] -
            kQ[0][1] * kQ[1][0] * kQ[2][2] - kQ[0][0] * kQ[1][2] * kQ[2][1];

    if(fDet < 0.0)
    {
        for(size_t iRow = 0; iRow < 3; iRow++)
            for(size_t iCol = 0; iCol < 3; iCol++)
                kQ[iRow][iCol] = -kQ[iRow][iCol];
    }

    // build "right" matrix R
    float kR[3][3];
    kR[0][0] = kQ[0][0] * m[0][0] + kQ[1][0] * m[1][0] + kQ[2][0] * m[2][0];
    kR[0][1] = kQ[0][0] * m[0][1] + kQ[1][0] * m[1][1] + kQ[2][0] * m[2][1];
    kR[1][1] = kQ[0][1] * m[0][1] + kQ[1][1] * m[1][1] + kQ[2][1] * m[2][1];
    kR[0][2] = kQ[0][0] * m[0][2] + kQ[1][0] * m[1][2] + kQ[2][0] * m[2][2];
    kR[1][2] = kQ[0][1] * m[0][2] + kQ[1][1] * m[1][2] + kQ[2][1] * m[2][2];
    kR[2][2] = kQ[0][2] * m[0][2] + kQ[1][2] * m[1][2] + kQ[2][2] * m[2][2];

    // the scaling component
    kD.x = kR[0][0];
    kD.y = kR[1][1];
    kD.z = kR[2][2];

    // the shear component
    float fInvD0 = 1.0f / kD.x;
    kU.x = kR[0][1] * fInvD0;
    kU.y = kR[0][2] * fInvD0;
    kU.z = kR[1][2] / kD.y;
}

MatDecompose Matrix4::decompose() const
{
    Vector3f position;
    Vector3f scale;
    Quaternion rotation;

    decompose(position, rotation, scale);

    MatDecompose md = {position, rotation, scale};

    return md;
}

void Matrix4::decompose(Vector3f& position, Quaternion& rotation, Vector3f& scale) const
{
    /*
     * Matrix decomposition code 
     *  from Ogre SDK : Matrix4::makeTransform
     * 
     * <https://bitbucket.org/sinbad/ogre/src/4aa2ca2384f4/OgreMain/src/OgreMatrix4.cpp>
     */

    float m3x3[3][3] = {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    };

    extractMat3((float*)&m3x3[0]);

    float matQ[3][3] = {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    };

    Vector3f vecU;

    qduDecomposition(m3x3, matQ, scale, vecU);

    Matrix4 rotmat;
    rotmat.importMat3((float*)&matQ[0]);

    rotation.setMatrix(rotmat);

    position = getPos();
}

void Matrix4::extractMat3(float* m3x3) const
{
    m3x3[0] = values[0];
    m3x3[1] = values[1];
    m3x3[2] = values[2];

    m3x3[3] = values[4];
    m3x3[4] = values[5];
    m3x3[5] = values[6];

    m3x3[6] = values[8];
    m3x3[7] = values[9];
    m3x3[8] = values[10];
}

void Matrix4::importMat3(float* m3x3)
{
    values[ 0] = m3x3[0];
    values[ 1] = m3x3[1];
    values[ 2] = m3x3[2];

    values[ 4] = m3x3[3];
    values[ 5] = m3x3[4];
    values[ 6] = m3x3[5];

    values[ 8] = m3x3[6];
    values[ 9] = m3x3[7];
    values[10] = m3x3[8];
}

Matrix4::operator const float*() const
{
    return values;
}

Matrix4::operator float*()
{
    return values;
}

float Matrix4::operator()(int i, int j)const
{
    return values[i * 4 + j];
}

float Matrix4::operator[](int i)const
{
    return values[i];
}

float & Matrix4::operator()(int i, int j)
{
    return values[i * 4 + j];
}

float & Matrix4::operator[](int i)
{
    return values[i];
}

Matrix4 Matrix4::operator*(const Matrix4& mat) const
{
    Matrix4 ret;

    ret.values[ 0] = (values[ 0] * mat.values[ 0]) + (values[ 1] * mat.values[ 4]) + (values[ 2] * mat.values[ 8]) + (values[ 3] * mat.values[12]);
    ret.values[ 1] = (values[ 0] * mat.values[ 1]) + (values[ 1] * mat.values[ 5]) + (values[ 2] * mat.values[ 9]) + (values[ 3] * mat.values[13]);
    ret.values[ 2] = (values[ 0] * mat.values[ 2]) + (values[ 1] * mat.values[ 6]) + (values[ 2] * mat.values[10]) + (values[ 3] * mat.values[14]);
    ret.values[ 3] = (values[ 0] * mat.values[ 3]) + (values[ 1] * mat.values[ 7]) + (values[ 2] * mat.values[11]) + (values[ 3] * mat.values[15]);

    ret.values[ 4] = (values[ 4] * mat.values[ 0]) + (values[ 5] * mat.values[ 4]) + (values[ 6] * mat.values[ 8]) + (values[ 7] * mat.values[12]);
    ret.values[ 5] = (values[ 4] * mat.values[ 1]) + (values[ 5] * mat.values[ 5]) + (values[ 6] * mat.values[ 9]) + (values[ 7] * mat.values[13]);
    ret.values[ 6] = (values[ 4] * mat.values[ 2]) + (values[ 5] * mat.values[ 6]) + (values[ 6] * mat.values[10]) + (values[ 7] * mat.values[14]);
    ret.values[ 7] = (values[ 4] * mat.values[ 3]) + (values[ 5] * mat.values[ 7]) + (values[ 6] * mat.values[11]) + (values[ 7] * mat.values[15]);

    ret.values[ 8] = (values[ 8] * mat.values[ 0]) + (values[ 9] * mat.values[ 4]) + (values[10] * mat.values[ 8]) + (values[11] * mat.values[12]);
    ret.values[ 9] = (values[ 8] * mat.values[ 1]) + (values[ 9] * mat.values[ 5]) + (values[10] * mat.values[ 9]) + (values[11] * mat.values[13]);
    ret.values[10] = (values[ 8] * mat.values[ 2]) + (values[ 9] * mat.values[ 6]) + (values[10] * mat.values[10]) + (values[11] * mat.values[14]);
    ret.values[11] = (values[ 8] * mat.values[ 3]) + (values[ 9] * mat.values[ 7]) + (values[10] * mat.values[11]) + (values[11] * mat.values[15]);

    ret.values[12] = (values[12] * mat.values[ 0]) + (values[13] * mat.values[ 4]) + (values[14] * mat.values[ 8]) + (values[15] * mat.values[12]);
    ret.values[13] = (values[12] * mat.values[ 1]) + (values[13] * mat.values[ 5]) + (values[14] * mat.values[ 9]) + (values[15] * mat.values[13]);
    ret.values[14] = (values[12] * mat.values[ 2]) + (values[13] * mat.values[ 6]) + (values[14] * mat.values[10]) + (values[15] * mat.values[14]);
    ret.values[15] = (values[12] * mat.values[ 3]) + (values[13] * mat.values[ 7]) + (values[14] * mat.values[11]) + (values[15] * mat.values[15]);

    return ret;
}

Matrix4 & Matrix4::operator*=(const Matrix4& mat)
{
    *this = *this * mat;
    return *this;
}

Vector3f Matrix4::operator*(const Vector3f& vec) const
{
    return Vector3f(values[0] * vec[0] + values[4] * vec[1] + values[8] * vec[2] + values[12],
                    values[1] * vec[0] + values[5] * vec[1] + values[9] * vec[2] + values[13],
                    values[2] * vec[0] + values[6] * vec[1] + values[10] * vec[2] + values[14]);
}

Matrix4 Matrix4::translate(Matrix4 mat, Vector3f pos)
{
    mat.translate(pos);
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
