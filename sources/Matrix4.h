/*
 * File:   Matrix4.h
 * Author: b4n92uid
 *
 * Created on 19 avril 2011, 15:18
 */

#ifndef MATRIX4_H
#define	MATRIX4_H

#include <string>
#include <sstream>

namespace tbe
{

template<typename T> class Vector3;
typedef Vector3<float> Vector3f;

class Quaternion;

class Matrix4
{
public:

    Matrix4();
    Matrix4(const float fmatrix[16]);
    Matrix4(const Vector3f& pos);
    Matrix4(std::string exp);
    Matrix4(const Matrix4& copy);

    /// Mets la matrice d'identité
    void identity();
    void transpose();

    /// Operateur d'assignement
    bool operator=(const Matrix4& copy);

    /// Mets une position a la matrice
    void setPos(Vector3f value);

    /// Renvoi la position de la matrice
    Vector3f getPos() const;

    /// Mets une taille a la matrice
    void setScale(Vector3f value);

    /// Renvoi la taille de la matrice
    Vector3f getScale() const;

    /// Mets une rotation a la matrice AxisAngle
    void setRotate(float angle, Vector3f axe);

    /// Mets une rotation X a la matrice Euler
    void setRotateX(float v);

    /// Mets une rotation Y a la matrice Euler
    void setRotateY(float v);

    /// Mets une rotation Z a la matrice Euler
    void setRotateZ(float v);

    /// Mets une rotation a la matrice Euler
    void setRotate(Vector3f v);

    /// Mets une rotation a la matrice Quaternion
    void setRotate(Quaternion rotation);

    /// Renvoi la rotation de la matrice
    Quaternion getRotate() const;

    operator Vector3f();

    operator const float*() const;
    operator float*();

    float operator()(int i, int j)const;
    float operator[](int i)const;

    float & operator()(int i, int j);
    float & operator[](int i);

    Matrix4 & operator*=(const Matrix4& mat);
    Matrix4 operator*(const Matrix4& mat) const;

    Vector3f operator*=(const Vector3f& vec);
    Vector3f operator*(const Vector3f& vec) const;

    void translate(Vector3f pos);

    void rotate(float angle, Vector3f axe);
    void rotate(Vector3f euler);
    void rotate(Quaternion rotation);

    void scale(Vector3f scale);

    static Matrix4 transpose(Matrix4 mat);

    static Matrix4 translate(Matrix4 mat, Vector3f pos);

    static Matrix4 rotate(Matrix4 mat, float angle, Vector3f axe);
    static Matrix4 rotate(Matrix4 mat, Quaternion quat);

    static Matrix4 scale(Matrix4 mat, Vector3f pos);

    friend std::istream & operator >>(std::istream& stream, Matrix4& mat)
    {
        char sep;
        for(unsigned i = 0; i < 15; i++)
            stream >> mat[i] >> sep;

        stream >> mat[15];

        return stream;
    }

    friend std::ostream & operator <<(std::ostream& stream, Matrix4 mat)
    {
        for(unsigned i = 0; i < 15; i++)
            stream << mat[i] << ",";

        stream << mat[15];

        return stream;
    }

    /**
     * Conversion d'une matrice 4x4 en chaine de caracteres
     *  au format "0.1, 0.2, 0.3[...]"
     */
    std::string toStr(char sep = ',') const
    {
        std::stringstream stream;
        for(unsigned i = 0; i < 15; i++)
            stream << m_matrix[i] << sep;

        stream << m_matrix[15];

        return stream.str();
    }

    /**
     * Conversion d'une matrice 4x4 en chaine de caracteres
     *  au format "0.1, 0.2, 0.3[...]"
     */
    void fromStr(const std::string& str, bool withsep = false)
    {
        char sep;
        std::stringstream stream(str);

        if(withsep)
            for(unsigned i = 0; i < 15; i++)
                stream >> m_matrix[i] >> sep;
        else
            for(unsigned i = 0; i < 15; i++)
                stream >> m_matrix[i];

        stream >> m_matrix[15];
    }

private:
    float m_matrix[16];
};

}

#endif	/* MATRIX4_H */

