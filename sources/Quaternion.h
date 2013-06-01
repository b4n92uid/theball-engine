/*
 * File:   Quaternion.h
 * Author: b4n92uid
 *
 * Created on 19 avril 2011, 15:25
 */

#ifndef QUATERNION_H
#define	QUATERNION_H

#include <iostream>
#include <sstream>

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
    Quaternion(float w, float x, float y, float z);
    Quaternion(const Matrix4& matrix);
    Quaternion(const Vector3f& euler);
    Quaternion(float angle, const Vector3f& axe);

    Quaternion& operator=(const Quaternion& quat);
    bool operator==(const Quaternion& quat);

    /**
     * Spécifier la rotation é l'aide d'un angle (exprimer en radian)
     *  et d'un axe autour du quelle la rotation decera étre éffectuer
     */
    void setAxisAngle(float angle, const Vector3f& axe);
    Vector4f getAxisAngle() const;

    /**
     * Spécifier la rotatino é l'aide d'un vecteur d'Euler
     */
    void setEuler(const Vector3f& euler);
    Vector3f getEuler() const;

    /**
     * Spécfier la rotation é l'aide d'un matrice de rotation,
     *  la matrice ne devera cotenir que la rotation
     */
    void setMatrix(const Matrix4& matrix);
    Matrix4 getMatrix() const;

    /**
     * Réinitialise la Quaternion é l'identité
     */
    Quaternion& identity();

    /**
     * Normalise la Quaternion [-1;1]
     */
    Quaternion& normalize();

    /**
     * Conjuge la Quaternion
     */
    Quaternion& conjugate();

    /**
     * Renvois la longeur de la Quaternion
     */
    float getMagnitude() const;

    /**
     * Renvois une copie de la Quaternion normaliser
     */
    Quaternion getNormalize() const;

    /**
     * Renvois une copie de la Quaternion conjuger
     */
    Quaternion getConjugate() const;

    Quaternion operator*(const Quaternion& rv) const;
    Quaternion& operator*=(const Quaternion& quat);

    Vector3f operator*(const Vector3f& vec) const;

    friend std::istream & operator >>(std::istream& stream, Quaternion& quat)
    {
        char sep;
        return stream >> quat.x >> sep >> quat.y >> sep >> quat.z >> sep >> quat.w;
    }

    friend std::ostream & operator<<(std::ostream& out, Quaternion quat)
    {
        return out << quat.x << ", " << quat.y << ", " << quat.z << ", " << quat.w;
    }

    /**
     * Conversion d'une quaternion en chaine de caracteres
     *  au format "x, y, z, w"
     */
    std::string toStr(char sep = ',') const
    {
        std::stringstream stream;
        stream << x << sep << y << sep << z << sep << w;

        return stream.str();
    }

    /**
     * Interprétation d'une chaine de caracteres en quaternion
     *  depuis le format "x, y, z, w"
     */
    Quaternion& fromStr(std::string str, bool withsep = true)
    {
        std::stringstream ss(str);

        if(withsep)
        {
            char sep;
            ss >> x >> sep >> y >> sep >> z >> sep >> w;
        }
        else
        {
            ss >> x >> y >> z >> w;
        }

        return *this;
    }

    float x, y, z, w;
};

}

#endif	/* QUATERNION_H */

