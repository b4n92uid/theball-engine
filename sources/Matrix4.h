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

    Matrix4(float, float, float, float,
            float, float, float, float,
            float, float, float, float,
            float, float, float, float);

    /// Mets à la matrice d'identité
    void identity();
    
    // Transpose la matrice
    void transpose();

    /// Operateur d'assignement
    bool operator=(const Matrix4& copy);

    /// Mets une position a la matrice
    void setPos(Vector3f value);

    /// Renvoi la position de la matrice
    Vector3f getPos() const;

    /// Efféctue une translation sur la matrice
    Matrix4& translate(const Vector3f& pos);

    /// Efféctue une rotation sur la matrice
    Matrix4& rotate(const Quaternion& rotation);

    /// Efféctue une mise a l'échelle sur la matrice
    Matrix4& scale(const Vector3f& scale);

    /**
     * Applique une transformation sur la matrice dans l'ordre:
     * 
     *  1. Scale
     *  2. Rotation
     *  3. Translation
     */
    void transform(Vector3f position, Quaternion rotation, Vector3f scale);

    /**
     * Décomposition de la matrice avec l'algorithme 
     *  de Gram-Schmidt (l'algorithm QR).
     */
    void decompose(Vector3f& position, Quaternion& rotation, Vector3f& scale);

    /// Exporte les 3 premierer linge et colone en matrice 3x3 (float[3][3])
    void extractMat3(float* m3x3);

    /// Importe les valeur depuis une matrice 3x3 (float[3][3])
    void importMat3(float* m3x3);

    operator const float*() const;
    operator float*();

    float operator()(int i, int j)const;
    float operator[](int i)const;

    float& operator()(int i, int j);
    float& operator[](int i);

    Matrix4& operator*=(const Matrix4& mat);
    Matrix4 operator*(const Matrix4& mat) const;

    Vector3f operator*(const Vector3f& vec) const;

    static Matrix4 transpose(Matrix4 mat);

    static Matrix4 translate(Matrix4 mat, Vector3f pos);
    static Matrix4 rotate(Matrix4 mat, Quaternion quat);
    static Matrix4 scale(Matrix4 mat, Vector3f pos);

    /**
     * Gestion du flux d'entrer sur la matrice
     */
    friend std::istream & operator >>(std::istream& stream, Matrix4& mat)
    {
        char sep;
        for(unsigned i = 0; i < 15; i++)
            stream >> mat[i] >> sep;

        stream >> mat[15];

        return stream;
    }

    /**
     * Gestion du flux de sortie sur la matrice
     */
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
            stream << values[i] << sep;

        stream << values[15];

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
                stream >> values[i] >> sep;
        else
            for(unsigned i = 0; i < 15; i++)
                stream >> values[i];

        stream >> values[15];
    }

private:
    float values[16];
};

}

#endif	/* MATRIX4_H */

