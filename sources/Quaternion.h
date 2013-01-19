/*
 * File:   Quaternion.h
 * Author: b4n92uid
 *
 * Created on 19 avril 2011, 15:25
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
    Quaternion(float w, float x, float y, float z);
    Quaternion(const Matrix4& matrix);
    Quaternion(const Vector3f& euler);
    Quaternion(float angle, const Vector3f& axe);

    /**
     * Spécifier la rotation â l'aide d'un angle (exprimer en radian)
     *  et d'un axe autour du quelle la rotation decera être éffectuer
     */
    void setAxisAngle(float angle, const Vector3f& axe);
    Vector4f getAxisAngle() const;

    /**
     * Spécifier la rotatino â l'aide d'un vecteur d'Euler
     */
    void setEuler(const Vector3f& euler);
    Vector3f getEuler() const;

    /**
     * Spécfier la rotation â l'aide d'un matrice de rotation,
     *  la matrice ne devera cotenir que la rotation
     */
    void setMatrix(const Matrix4& matrix);
    Matrix4 getMatrix() const;

    /**
     * Réinitialise la Quaternion â l'identité
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

    friend std::ostream & operator<<(std::ostream& out, Quaternion& quat)
    {
        return out << quat.x << ", " << quat.y << ", " << quat.z << ", " << quat.w;
    }

    float x, y, z, w;
};

}

#endif	/* QUATERNION_H */

