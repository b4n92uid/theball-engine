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

inline void radianClamp(float& angle)
{
    if(angle > M_PI)
        angle = -(M_PI - fmod(angle, M_PI));
}

inline void radianClamp(Vector3f& euler)
{
    radianClamp(euler.x);
    radianClamp(euler.y);
    radianClamp(euler.z);
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

    //return (min + ((float) std::rand() / RAND_MAX * (max - min + 1.0)));
    return (min + ((float)std::rand() / RAND_MAX * (max - min)));
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

    return Vector3f(
                    rand(min.x, max.x),
                    rand(min.y, max.y),
                    rand(min.z, max.z)
                    );
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
void round(T& value, const T& unit)
{
    T sign = value >= 0 ? 1 : -1;

    value = std::abs(value);

    T diff = value % unit;

    if(diff >= unit / T(2))
        value += unit - diff;
    else
        value -= diff;

    value *= sign;
}

/**
 * Spécialisation de round() pour Vector2f
 *
 * @param value
 * @param unit
 * @return
 */
inline void round(Vector2f& value)
{
    value.x = roundf(value.x);
    value.y = roundf(value.y);
}

/**
 * Spécialisation de round() pour Vector3f
 *
 * @param value
 * @param unit
 * @return
 */
inline void round(Vector3f& value)
{
    value.x = roundf(value.x);
    value.y = roundf(value.z);
    value.z = roundf(value.y);
}

/**
 * Spécialisation de round() pour Vector4f
 *
 * @param value
 * @param unit
 * @return
 */
inline void round(Vector4f& value)
{
    value.x = roundf(value.x);
    value.y = roundf(value.z);
    value.z = roundf(value.y);
    value.w = roundf(value.w);
}

/**
 * Spécialisation de round() pour float
 *
 * @param value
 * @param unit
 * @return
 */
template<>
inline void round<float>(float& value, const float& unit)
{
    float sign = value >= 0 ? 1 : -1;

    value = std::abs(value);

    float diff = floatModulo(value, unit);

    if(diff >= unit / 2.0f)
        value += unit - diff;
    else
        value -= diff;

    value *= sign;
}

/**
 * round() sur Vector2
 *
 * @param value
 * @param unit
 * @return
 */
template<typename T>
void round(Vector2<T>& value, const Vector2<T>& unit)
{
    round(value.x, unit.x);
    round(value.y, unit.y);
}

/**
 * round() sur Vector3
 *
 * @param value
 * @param unit
 * @return
 */
template<typename T>
void round(Vector3<T>& value, const Vector3<T>& unit)
{
    round(value.x, unit.x);
    round(value.y, unit.y);
    round(value.z, unit.z);
}

}
}

#endif	/* _MATHEMATICS_H */

