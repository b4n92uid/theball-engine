/* 
 * File:   tbe_tools.h
 * Author: Administrateur
 *
 * Created on 21 ao�t 2009, 21:34
 */

#ifndef _TBE_TOOLS_H
#define	_TBE_TOOLS_H

#include <algorithm>
#include <iostream>
#include <sstream>
#include <map>

#include "Mathematics.h"
#include "AABB.h"

namespace tbe
{
namespace tools
{

/**
 * Renvois la premier puissance-deux apres 'v'
 *
 * @param v
 * @return
 */
template<typename T> inline Vector2<T> next_p2(Vector2<T> v)
{
    Vector2<T> rval = 2;
    // rval<<=1 Is A Prettier Way Of Writing rval*=2;
    while(rval.x < v.x) rval.x <<= 1;
    while(rval.y < v.y) rval.y <<= 1;

    return rval;
}

/**
 * Renvois la premier puissance-deux apres 'v'
 *
 * @param a
 * @return
 */
template<typename T> inline T next_p2(T v)
{
    T rval = 2;
    // rval<<=1 Is A Prettier Way Of Writing rval*=2;
    while(rval < v) rval <<= 1;

    return rval;
}

/**
 * Retourne la chaine 'name' sous format compatible Unix
 *
 * @param name
 * @return
 */
inline std::string UnixName(std::string name)
{
    typedef std::map<char, char> repMap;

    for(unsigned i = 0; i < name.size(); i++)
        name[i] = tolower(name[i]);

    repMap replacement;
    replacement[' '] = '-';
    replacement['�'] = 'e';
    replacement['�'] = 'e';

    for(repMap::iterator it = replacement.begin(); it != replacement.end(); it++)
        std::replace(name.begin(), name.end(), it->first, it->second);

    return name;
}

/**
 * G�n�re un nom unique pour map
 *
 * @param map
 * @param prefix
 * @return
 */
template<typename T2>
std::string NameGen(std::map<std::string, T2>& map, std::string prefix = std::string())
{
    std::stringstream name(prefix + "0");

    for(unsigned id = 1; map.find(name.str()) != map.end(); id++)
    {
        name.str("");
        name << prefix << id;
    }

    return name.str();
}

/**
 * G�n�re un nom unique pour un tableau de noeud (Node)
 *
 * @param array
 * @param prefix
 * @return
 */
template<typename T>
std::string NameGen(std::vector<T>& array, std::string prefix = std::string())
{
    std::stringstream name(prefix + "0");

    for(unsigned id = 1; true; id++)
    {
        bool containe = false;

        for(unsigned i = 0; i < array.size(); i++)
            if(array[i]->GetName() == name.str())
            {
                containe = true;
                break;
            }

        if(!containe)
            return name.str();

        name.str("");
        name << prefix << id;
    }

    return name.str();
}

/**
 * Conversion num�rique vers une chaine de characters
 *
 * @param numeric
 * @return
 */
template <typename T> std::string numToStr(T numeric)
{
    std::stringstream ss;
    ss << numeric;
    return ss.str();
}

/**
 * Conversion de chaine de characters vers valeur num�rique
 *
 * @param str
 * @return
 */
template <typename T> T StrToNum(std::string str)
{
    T numeric;
    std::stringstream ss(str);
    ss >> numeric;
    return numeric;
}

/**
 * Renvoi un nombre entier al�atoir entre min et max
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
 * Renvoi un nombre flotant al�atoir entre min et max
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
    return(min + ((float)std::rand() / RAND_MAX * (max - min)));
}

/**
 * Renvoi un vectur al�atoir situer entre min et max
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
 * Renvoi un vectur al�atoir situer a l'interieur de aabb
 *
 * @param min
 * @param max
 * @return
 */
inline Vector3f rand(AABB aabb)
{
    return rand(aabb.min, aabb.max);
}

/**
 * fmod() implementation
 * 
 * @param lvalue
 * @param rvalue
 * @return 
 */
inline float ModuloFloat(float lvalue, float rvalue)
{
    if(lvalue - rvalue > 0)
        return ModuloFloat(lvalue - rvalue, rvalue);
    else
        return lvalue;
}

/**
 * Renvois le valeur arrodis par unit
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
 * Sp�cialisation de round pour float
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

    float diff = ModuloFloat(value, unit);

    if(diff >= unit / 2.0f)
        value += unit - diff;
    else
        value -= diff;

    value *= sign;
}

/**
 * round sur Vector2
 *
 * @param value
 * @param unit
 * @return
 */
template<typename T>
void round(Vector2<T>& value, const Vector2<T>& unit)
{
    tools::round(value.x, unit.x);
    tools::round(value.y, unit.y);
}

/**
 * round sur Vector3
 *
 * @param value
 * @param unit
 * @return
 */
template<typename T>
void round(Vector3<T>& value, const Vector3<T>& unit)
{
    tools::round(value.x, unit.x);
    tools::round(value.y, unit.y);
    tools::round(value.z, unit.z);
}

}
}
#endif	/* _TBE_TOOLS_H */

