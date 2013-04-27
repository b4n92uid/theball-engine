/*
 * File:   Mathematics.h
 * Author: b4n92uid
 *
 * Created on 24 janvier 2010, 19:02
 */

#ifndef _MATHEMATICS_H
#define	_MATHEMATICS_H

#include <iostream>
#include <cmath>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "AABB.h"
#include "Tools.h"

namespace tbe
{
namespace math
{

/**
 * Conversion depuis un vecteur a 3 dimension à un vecteur a 2 dimension
 */
template<typename T> inline tbe::Vector2<T> vec32(tbe::Vector3<T> vec3)
{
    return tbe::Vector2<T > (vec3.x, vec3.y);
}

/**
 * Conversion depuis un vecteur a 2 dimension à un vecteur a 3 dimension
 */
template<typename T> inline tbe::Vector2<T> vec23(tbe::Vector2<T> vec2)
{
    return tbe::Vector3<T > (vec2.x, vec2.y, 0);
}

/**
 * Conversion depuis un vecteur a 4 dimension à un vecteur a 2 dimension
 */
template<typename T> inline tbe::Vector2<T> vec42(tbe::Vector4<T> vec4)
{
    return tbe::Vector2<T > (vec4.x, vec4.y);
}

/**
 * Conversion depuis un vecteur a 2 dimension à un vecteur a 4 dimension
 */
template<typename T> inline tbe::Vector4<T> vec24(tbe::Vector2<T> vec2)
{
    return tbe::Vector4<T > (vec2.x, vec2.y, 0, 1);
}

/**
 * Conversion depuis un vecteur a 4 dimension à un vecteur a 3 dimension
 */
template<typename T> inline tbe::Vector3<T> vec43(tbe::Vector4<T> vec4)
{
    return tbe::Vector3<T > (vec4.x, vec4.y, vec4.z);
}

/**
 * Conversion depuis un vecteur a 3 dimension à un vecteur a 4 dimension
 */
template<typename T> inline tbe::Vector4<T> vec34(tbe::Vector3<T> vec3)
{
    return tbe::Vector4<T > (vec3.x, vec3.y, vec3.z, 1);
}

inline float radianClamp(float angle)
{
    if(angle > M_PI)
        angle = -(M_PI - fmod(angle, M_PI));

    return angle;
}

inline Vector3f radianClamp(const Vector3f& euler)
{
    Vector3f v;

    v.x = radianClamp(euler.x);
    v.y = radianClamp(euler.y);
    v.z = radianClamp(euler.z);

    return v;
}

#define FLOAT_TEST_FACTOR 0.001

/**
 * Test d'égaliter sur float
 *  le parametre factor définie le degres de précision de l'operation
 */
inline bool isEqual(float a, float b, float factor = FLOAT_TEST_FACTOR)
{
    return (fabs(a - b) < factor);
}

/**
 * Test d'égaliter sur Vecteur a 3 dimension avec un float
 *  le parametre factor définie le degres de précision de l'operation
 */
inline bool isEqual(Vector3f a, float b, float factor = FLOAT_TEST_FACTOR)
{
    return (isEqual(a.x, b, factor) && isEqual(a.y, b, factor) && isEqual(a.z, b, factor));
}

/**
 * Test d'égaliter sur Vecteur a 3 dimension avec autre vecteur
 *  le parametre factor définie le degres de précision de l'operation
 */
inline bool isEqual(Vector3f a, Vector3f b, float factor = FLOAT_TEST_FACTOR)
{
    return (isEqual(a.x, b.x, factor) && isEqual(a.y, b.y, factor) && isEqual(a.z, b.z, factor));
}

/**
 * Test d'égaliter sur Vecteur a 4 dimension avec un float
 *  le parametre factor définie le degres de précision de l'operation
 */
inline bool isEqual(Vector4f a, float b, float factor = FLOAT_TEST_FACTOR)
{
    return (isEqual(a.x, b, factor) && isEqual(a.y, b, factor) && isEqual(a.z, b, factor) && isEqual(a.w, b, factor));
}

/**
 * Test d'égaliter sur Vecteur a 4 dimension avec autre vecteur
 *  le parametre factor définie le degres de précision de l'operation
 */
inline bool isEqual(Vector4f a, Vector4f b, float factor = FLOAT_TEST_FACTOR)
{
    return (isEqual(a.x, b.x, factor) && isEqual(a.y, b.y, factor) && isEqual(a.z, b.z, factor) && isEqual(a.w, b.w, factor));
}

/**
 * Test si le valeur flotante de est à zéro
 *  le parametre factor définie le degres de précision de l'operation
 */
inline bool isZero(float a, float factor = FLOAT_TEST_FACTOR)
{
    return (a > -factor && a < factor);
}

/**
 * Test si une des valeurs du vecteur est à zéro
 *  le parametre factor définie le degres de précision de l'operation
 */
inline bool isAnyZero(Vector3f a, float factor = FLOAT_TEST_FACTOR)
{
    return (isZero(a.x, factor) || isZero(a.y, factor) || isZero(a.z, factor));
}

/**
 * Test si toute les valeurs du vecteur sont à zéro
 *  le parametre factor définie le degres de précision de l'operation
 */
inline bool isZero(Vector3f a, float factor = FLOAT_TEST_FACTOR)
{
    return (isZero(a.x, factor) && isZero(a.y, factor) && isZero(a.z, factor));
}

/**
 * Test si une des valeurs du vecteur est à zéro
 *  le parametre factor définie le degres de précision de l'operation
 */
inline bool isAnyZero(Vector2f a, float factor = FLOAT_TEST_FACTOR)
{
    return (isZero(a.x, factor) || isZero(a.y, factor));
}

/**
 * Test si toute les valeurs du vecteur sont à zéro
 *  le parametre factor définie le degres de précision de l'operation
 */
inline bool isZero(Vector2f a, float factor = FLOAT_TEST_FACTOR)
{
    return (isZero(a.x, factor) && isZero(a.y, factor));
}

/**
 * Indique si la valeur 'value' est de puissance 2
 *
 * @param value
 * @return
 */
template<typename T> inline bool isPow2(T value)
{
    return (value != 0) && !(value & (value - 1));
}

/**
 * Renvois la premiere valeur de puissance-deux apres 'v'
 *
 * Implementer depuis le tutoriel de NeHe sur Freetype:
 * <http://nehe.gamedev.net/tutorial/freetype_fonts_in_opengl/24001/>
 *
 * @param a
 * @return
 */
template<typename T> inline T nextPow2(T v)
{
    T rval = 2;
    // rval<<=1 Is A Prettier Way Of Writing rval*=2;
    while(rval < v) rval <<= 1;

    return rval;
}

/**
 * Renvois la valeur 'value' si elle se situe entre min et max,
 *  sinon elle renvois une des extrémité
 *
 * @param value
 * @param min
 * @param max
 * @return
 */
template<typename T> T clamp(const T& value, const T& min, const T& max)
{
    return std::max(min, std::min(value, max));
}

/**
 * Renvois la premier puissance-deux après 'v'
 *
 * @param v
 * @return
 */
template<typename T> inline Vector2<T> nextPow2(Vector2<T> v)
{
    Vector2<T> rval = 2;

    // rval<<=1 Is A Prettier Way Of Writing rval*=2;
    while(rval.x < v.x) rval.x <<= 1;
    while(rval.y < v.y) rval.y <<= 1;

    return rval;
}

/**
 * Renvoi un nombre entier aléatoir entre min et max
 *
 * @param min
 * @param max
 * @return
 */
inline int rand(int min, int max)
{
    if(!min && !max)
        return 0;

    return std::rand() % max + min;
}

/**
 * Renvoi un nombre flotant aléatoir entre min et max
 *
 * @param min
 * @param max
 * @return
 */
inline float rand(float min, float max)
{
    if(!min && !max)
        return 0;

    return (min + ((float) std::rand() / RAND_MAX * (max - min)));
}

/**
 * Renvoi un vectur aléatoir situer entre min et max
 *
 * @param min
 * @param max
 * @return
 */
inline Vector3f rand(Vector3f min, Vector3f max)
{
    return Vector3f(rand(min.x, max.x),
                    rand(min.y, max.y),
                    rand(min.z, max.z));
}

/**
 * fmod() implementation
 *
 * @param lvalue
 * @param rvalue
 * @return
 */
inline float floatModulo(float lvalue, float rvalue)
{
    if(lvalue - rvalue > 0)
        return floatModulo(lvalue - rvalue, rvalue);
    else
        return lvalue;
}

/**
 * Renvois le valeur arrodis de 'value' par 'unit'
 *
 * @param value
 * @param unit
 * @return
 */
template<typename T>
inline T round(const T& value, const T& unit)
{
    T v;

    T sign = value >= 0 ? 1 : -1;

    v = std::abs(value);

    T diff = v % unit;

    if(diff >= unit / T(2))
        v += unit - diff;
    else
        v -= diff;

    v *= sign;

    return v;
}

/**
 * Spécialisation de round() pour float
 *
 * @param value
 * @param unit
 * @return
 */
template<>
inline float round<float>(const float& value, const float& unit)
{
    float v;

    float sign = value >= 0 ? 1 : -1;

    v = std::abs(value);

    float diff = floatModulo(v, unit);

    if(diff >= unit / 2.0f)
        v += unit - diff;
    else
        v -= diff;

    v *= sign;

    return v;
}

/**
 * round() sur Vector2
 *
 * @param value
 * @param unit
 * @return
 */
template<typename T>
inline Vector2<T> round(const Vector2<T>& value, const Vector2<T>& unit)
{
    Vector2<T> v;

    v.x = round(value.x, unit.x);
    v.y = round(value.y, unit.y);

    return v;
}

/**
 * round() sur Vector3
 *
 * @param value
 * @param unit
 * @return
 */
template<typename T>
inline Vector3<T> round(const Vector3<T>& value, const Vector3<T>& unit)
{
    Vector3<T> v;

    v.x = round(value.x, unit.x);
    v.y = round(value.y, unit.y);
    v.z = round(value.z, unit.z);

    return v;
}

/**
 * round() sur Vector4
 *
 * @param value
 * @param unit
 * @return
 */
template<typename T>
inline Vector4<T> round(const Vector4<T>& value, const Vector4<T>& unit)
{
    Vector4<T> v;

    v.x = round(value.x, unit.x);
    v.y = round(value.y, unit.y);
    v.z = round(value.z, unit.z);
    v.w = round(value.w, unit.w);

    return v;
}

/**
 * Spécialisation de round() pour Vector2f
 *
 * @param value
 * @param unit
 * @return
 */
inline Vector2f round(Vector2f value)
{
    value.x = roundf(value.x);
    value.y = roundf(value.y);

    return value;
}

/**
 * Spécialisation de round() pour Vector3f
 *
 * @param value
 * @param unit
 * @return
 */
inline Vector3f round(Vector3f value)
{
    value.x = roundf(value.x);
    value.y = roundf(value.y);
    value.z = roundf(value.z);

    return value;
}

/**
 * Spécialisation de round() pour Vector4f
 *
 * @param value
 * @param unit
 * @return
 */
inline Vector4f round(Vector4f value)
{
    value.x = roundf(value.x);
    value.y = roundf(value.y);
    value.z = roundf(value.z);
    value.w = roundf(value.w);

    return value;
}

/**
 * Fast inverse square root:
 * Implemented from Quake 3 source
 *
 * <http://en.wikipedia.org/wiki/Fast_inverse_square_root>
 */
inline float reverseSqrt(float number)
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *(long*) &y; // evil floating point bit level hacking
    i = 0x5f3759df - (i >> 1); // what the fuck?
    y = *(float*) &i;
    y = y * (threehalfs - (x2 * y * y)); // 1st iteration
    y = y * (threehalfs - (x2 * y * y)); // 2nd iteration, this can be removed

    return y;
}

inline Matrix4 orthographicMatrix(float left, float right, float bottom, float top, float znear, float zfar)
{
    Matrix4 result;

    result(0, 0) = 2 / (right - left);
    result(1, 1) = 2 / (top - bottom);
    result(2, 2) = -2 / (zfar - znear);
    result(3, 3) = 1;

    result(3, 0) = -(right + left) / (right - left);
    result(3, 1) = -(top + bottom) / (top - bottom);
    result(3, 2) = -(zfar + znear) / (zfar - znear);

    return result;
}

inline Matrix4 perspectiveMatrix(float fovy, float aspect, float zNear, float zFar)
{
    // from GLM
    // <https://github.com/g-truc/glm/blob/0.9.4/glm/gtc/matrix_transform.inl>

    Matrix4 result;

    float range = tan((fovy / 2) * M_PI / 180) * zNear;
    float left = -range * aspect;
    float right = range * aspect;
    float bottom = -range;
    float top = range;

    result(0, 0) = (2 * zNear) / (right - left);
    result(1, 1) = (2 * zNear) / (top - bottom);
    result(2, 2) = -(zFar + zNear) / (zFar - zNear);
    result(2, 3) = -1;
    result(3, 2) = -(2 * zFar * zNear) / (zFar - zNear);

    return result;
}

inline Matrix4 lookAt(Vector3f eye, Vector3f center, Vector3f up)
{
    // from GLM
    // <https://github.com/g-truc/glm/blob/0.9.4/glm/gtc/matrix_transform.inl>

    Vector3f f = (center - eye).normalize();
    Vector3f u = Vector3f::normalize(up);
    Vector3f s = Vector3f::cross(f, u).normalize();
    u = Vector3f::cross(s, f);

    Matrix4 result;

    result(0, 0) = s.x;
    result(1, 0) = s.y;
    result(2, 0) = s.z;
    result(0, 1) = u.x;
    result(1, 1) = u.y;
    result(2, 1) = u.z;
    result(0, 2) = -f.x;
    result(1, 2) = -f.y;
    result(2, 2) = -f.z;
    result(3, 0) = -Vector3f::dot(s, eye);
    result(3, 1) = -Vector3f::dot(u, eye);
    result(3, 2) = Vector3f::dot(f, eye);

    return result;
}

}
}

#endif	/* _MATHEMATICS_H */
