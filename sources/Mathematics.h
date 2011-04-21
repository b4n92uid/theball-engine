/* 
 * File:   Mathematics.h
 * Author: b4n92uid
 *
 * Created on 24 janvier 2010, 19:02
 */

#ifndef _MATHEMATICS_H
#define	_MATHEMATICS_H

#include <cmath>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "Quaternion.h"

namespace tbe
{

template<typename T> inline tbe::Vector2<T> vec32(tbe::Vector3<T> vec3)
{
    return tbe::Vector2<T > (vec3.x, vec3.y);
}

template<typename T> inline tbe::Vector2<T> vec23(tbe::Vector2<T> vec2)
{
    return tbe::Vector3<T > (vec2.x, vec2.y, 0);
}

template<typename T> inline tbe::Vector2<T> vec42(tbe::Vector4<T> vec4)
{
    return tbe::Vector2<T > (vec4.x, vec4.y);
}

template<typename T> inline tbe::Vector4<T> vec24(tbe::Vector2<T> vec2)
{
    return tbe::Vector4<T > (vec2.x, vec2.y, 0, 1);
}

template<typename T> inline tbe::Vector3<T> vec43(tbe::Vector4<T> vec4)
{
    return tbe::Vector3<T > (vec4.x, vec4.y, vec4.z);
}

template<typename T> inline tbe::Vector4<T> vec34(tbe::Vector3<T> vec3)
{
    return tbe::Vector4<T > (vec3.x, vec3.y, vec3.z, 1);
}

}

#endif	/* _MATHEMATICS_H */

