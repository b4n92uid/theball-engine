/* 
 * File:   ../Matrix.h
 * Author: b4n92uid
 *
 * Created on 16 novembre 2009, 16:56
 */

#ifndef _MATRIX_H
#define	_MATRIX_H

#include "Vector3.h"

namespace tbe
{

/// \brief Template Matrice 4x4

template<typename T> class Matrix4
{
public:

    Matrix4()
    {
        Identity();
    }

    Matrix4(const T fmatrix[16])
    {
        for(unsigned i = 0; i < 16; i++)
            m_matrix[i] = fmatrix[i];
    }

    Matrix4(Vector3<T> pos, Vector3<T> scale = 1, Vector3<T> rotate = 0)
    {
        Identity();

        SetPos(pos);
        SetScale(scale);
        SetRotate(rotate);
    }

    Matrix4(std::string exp)
    {
        Identity();

        char sep;
        std::stringstream ss(exp);

        for(unsigned i = 0; i < 15; i++)
            ss >> m_matrix[i] >> sep;

        ss >> m_matrix[15];
    }

    Matrix4(const Matrix4& copy)
    {
        *this = copy;
    }

    /// Mets la matrice d'identité

    void Identity()
    {
        for(unsigned i = 0; i < 16; i++)
            m_matrix[i] = 0.0f;

        m_matrix[0] = m_matrix[5] = m_matrix[10] = m_matrix[15] = 1.0f;
    }

    void Transpose()
    {
        T t;

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

    /// Operateur d'assignement

    bool operator=(const Matrix4& copy)
    {
        for(unsigned i = 0; i < 16; i++)
            m_matrix[i] = copy.m_matrix[i];

        return true;
    }

    /// Renvoi la position de la matrice

    Vector3<T> GetPos() const
    {
        return Vector3<T > (m_matrix[12], m_matrix[13], m_matrix[14]);
    }

    /// Mets une position a la matrice

    void SetPos(Vector3<T> value)
    {
        m_matrix[12] = value.x;
        m_matrix[13] = value.y;
        m_matrix[14] = value.z;
    }


    /// Renvoi la taille de la matrice

    Vector3<T> GetScale() const
    {
        return Vector3<T > (m_matrix[0], m_matrix[5], m_matrix[10]);
    }


    /// Mets une taille a la matrice

    void SetScale(Vector3<T> value)
    {
        m_matrix[0] = value.x;
        m_matrix[5] = value.y;
        m_matrix[10] = value.z;
    }

    /// Renvoi la rotation de la matrice

    Vector3<T> GetRotate() const
    {
        return 0;
    }

    /// Mets une rotation a la matrice

    void SetRotateX(T v)
    {
        float cosinus = cos(v);
        float sinus = sin(v);

        Matrix4<T> mat;
        mat[ 5] = cosinus;
        mat[ 6] = -sinus;
        mat[ 9] = sinus;
        mat[10] = cosinus;

        *this *= mat;
    }

    void SetRotateY(T v)
    {
        float cosinus = cos(v);
        float sinus = sin(v);

        Matrix4<T> mat;
        mat[ 0] = cosinus;
        mat[ 2] = -sinus;
        mat[ 8] = sinus;
        mat[10] = cosinus;

        *this *= mat;
    }

    void SetRotateZ(T v)
    {
        float cosinus = cos(v);
        float sinus = sin(v);

        Matrix4<T> mat;
        mat[0] = cosinus;
        mat[1] = -sinus;
        mat[4] = sinus;
        mat[5] = cosinus;

        *this *= mat;
    }

    void SetRotate(Vector3<T> v)
    {
        SetRotateX(v.x);
        SetRotateY(v.y);
        SetRotateZ(v.z);
    }

    void SetRotate(float angle, Vector3<T> axe)
    {
        float c = cos(angle);
        float s = sin(angle);

        float &x = axe.x, &y = axe.y, &z = axe.z;

        Matrix4<T> rotation;
        rotation[ 0] = x * x * (1 - c) + c;
        rotation[ 1] = y * x * (1 - c) + z * s;
        rotation[ 2] = x * z * (1 - c) - y * s;
        rotation[ 3] = 0;

        rotation[ 4] = x * y * (1 - c) - z * s;
        rotation[ 5] = y * y * (1 - c) + c;
        rotation[ 6] = y * z * (1 - c) + x * s;
        rotation[ 7] = 0;

        rotation[ 8] = x * z * (1 - c) + y * s;
        rotation[ 9] = y * z * (1 - c) - x * s;
        rotation[10] = z * z * (1 - c) + c;
        rotation[11] = 0;

        rotation[12] = 0;
        rotation[13] = 0;
        rotation[14] = 0;
        rotation[15] = 1;

        *this *= rotation;
    }

    operator Vector3<T>()
    {
        return GetPos();
    }

    operator const T*() const
    {
        return m_matrix;
    }

    operator T*()
    {
        return m_matrix;
    }

    T & operator()(int i, int j)
    {
        return m_matrix[i * 4 + j];
    }

    T & operator[](int i)
    {
        return m_matrix[i];
    }

    Matrix4 operator*=(const Matrix4& mat)
    {
        *this = *this * mat;
        return *this;
    }

    Matrix4 operator*(const Matrix4& mat) const
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

    Vector3<T> operator*=(const Vector3<T>& vec)
    {
        *this = *this * vec;
        return *this;
    }

    Vector3<T> operator*(const Vector3<T>& vec) const
    {
        return Vector3<T > (
                m_matrix[0] * vec[0] + m_matrix[4] * vec[1] + m_matrix[8] * vec[2] + m_matrix[12],
                m_matrix[1] * vec[0] + m_matrix[5] * vec[1] + m_matrix[9] * vec[2] + m_matrix[13],
                m_matrix[2] * vec[0] + m_matrix[6] * vec[1] + m_matrix[10] * vec[2] + m_matrix[14]
                );
    }

private:
    T m_matrix[16];
};

typedef Matrix4<int> Matrix4i;
typedef Matrix4<float> Matrix4f;

}

#endif	/* _MATRIX_H */

