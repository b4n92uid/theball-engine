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

#include "Matrix.h"

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

template<typename T> inline std::istream & operator >>(std::istream& stream, tbe::Vector4<T>& vec)
{
    char sep;
    return stream >> vec.x >> sep >> vec.y >> sep >> vec.z >> sep >> vec.w;
}

template<typename T> inline std::istream & operator >>(std::istream& stream, tbe::Vector3<T>& vec)
{
    char sep;
    return stream >> vec.x >> sep >> vec.y >> sep >> vec.z;
}

template<typename T> inline std::istream & operator >>(std::istream& stream, tbe::Vector2<T>& vec)
{
    char sep;
    return stream >> vec.x >> sep >> vec.y;
}

template<typename T> inline std::istream & operator >>(std::istream& stream, tbe::Matrix4<T>& mat)
{
    char sep;
    for(unsigned i = 0; i < 15; i++)
        stream >> mat[i] >> sep;

    stream >> mat[15];

    return stream;
}

template<typename T> inline std::ostream & operator <<(std::ostream& stream, tbe::Vector4<T> vec)
{
    return stream << vec.x << "," << vec.y << "," << vec.z << "," << vec.w;
}

template<typename T> inline std::ostream & operator <<(std::ostream& stream, tbe::Vector3<T> vec)
{
    return stream << vec.x << "," << vec.y << "," << vec.z;
}

template<typename T> inline std::ostream & operator <<(std::ostream& stream, tbe::Vector2<T> vec)
{
    return stream << vec.x << "," << vec.y;
}

template<typename T> inline std::ostream & operator <<(std::ostream& stream, tbe::Matrix4<T> mat)
{
    for(unsigned i = 0; i < 15; i++)
        stream << mat[i] << ",";

    stream << mat[15];

    return stream;
}

template<typename T> inline std::string Matrix4ToStr(const Matrix4<T>& mat)
{
    std::stringstream stream;
    for(unsigned i = 0; i < 15; i++)
        stream << mat[i] << ",";

    stream << mat[15];

    return stream.str();
}

template<typename T> inline std::string Vector2ToStr(const Vector2<T>& vec)
{
    std::stringstream stream;
    stream << vec.x << ", " << vec.y;

    return stream.str();
}

template<typename T> inline std::string Vector3ToStr(const Vector3<T>& vec)
{
    std::stringstream stream;
    stream << vec.x << ", " << vec.y << ", " << vec.z;

    return stream.str();
}

template<typename T> inline std::string Vector4ToStr(const Vector4<T>& vec)
{
    std::stringstream stream;
    stream << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w;

    return stream.str();
}


}

#endif	/* _MATHEMATICS_H */

