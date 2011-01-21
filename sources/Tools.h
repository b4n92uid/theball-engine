/* 
 * File:   tbe_tools.h
 * Author: Administrateur
 *
 * Created on 21 août 2009, 21:34
 */

#ifndef _TBE_TOOLS_H
#define	_TBE_TOOLS_H

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <map>

#include "Mathematics.h"
#include "AABB.h"

namespace tbe
{
namespace tools
{

inline std::string pathname(std::string filename)
{
    unsigned pos = filename.find_last_of('\\');

    if(pos == std::string::npos)
        pos = filename.find_last_of('/');

    return filename.substr(0, pos);
}

inline std::string basename(std::string filename, bool withExt = true)
{
    unsigned pos = filename.find_last_of('\\');

    if(pos == std::string::npos)
        pos = filename.find_last_of('/');

    if(withExt)
        return filename.substr(pos + 1);
    else
        return filename.substr(pos + 1, filename.find_last_of('.'));
}

inline std::string makeRelatifTo(std::string absfile, const std::string& relfile)
{
    unsigned reppos;
    while((reppos = absfile.find('\\')) != std::string::npos)
        absfile.replace(reppos, 1, 1, '/');

    unsigned pos = absfile.find_last_of('\\');

    if(pos == std::string::npos)
        pos = absfile.find_last_of('/');

    return absfile.substr(0, pos + 1) + relfile;
}

template<typename T> T clamp(const T& value, const T& min, const T& max)
{
    return std::max(min, std::min(value, max));
}

template<typename T> void erase(std::vector<T>& vec, unsigned index)
{
    vec.erase(vec.begin() + index);
}

template<typename T, typename T2> void erase(std::vector<T>& vec, T2 val)
{
    vec.erase(std::find(vec.begin(), vec.end(), dynamic_cast<T2>(val)));
}

template<typename T> void erase(std::vector<T>& vec, T val)
{
    vec.erase(std::find(vec.begin(), vec.end(), val));
}

template<typename T, typename T2> T find(const std::vector<T>& vec, T2 val)
{
    typename std::vector<T>::const_iterator it = std::find(vec.begin(), vec.end(), val);
    return (it != vec.end()) ? *it : NULL;
}

template<typename T> bool find(const std::vector<T>& vec, T val)
{
    return std::find(vec.begin(), vec.end(), val) != vec.end();
}

inline void trimstr(std::string& buffer)
{
    using namespace std;

    for(string::iterator it = buffer.begin(); isspace(*it); it = buffer.begin())
        buffer.erase(it);

    for(string::iterator it = --buffer.end(); isspace(*it); it--)
        buffer.erase(it);
}

inline bool isspace(const std::string& buffer)
{
    for(unsigned i = 0; i < buffer.size(); i++)
        if(!std::isspace(buffer[i]))
            return false;

    return true;
}

inline bool getline(std::istream& stream, std::string& buffer)
{
    using namespace std;

    bool status = std::getline(stream, buffer);

    #ifdef __linux__
    for(string::iterator it = --buffer.end(); isspace(*it); it--)
        buffer.erase(it);
    #endif

    return status;
}

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
    replacement['é'] = 'e';
    replacement['è'] = 'e';

    for(repMap::iterator it = replacement.begin(); it != replacement.end(); ++it)
        std::replace(name.begin(), name.end(), it->first, it->second);

    return name;
}

/**
 * Génére un nom unique pour map
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
 * Génére un nom unique pour un tableau de noeud (Node)
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
 * Conversion numérique vers une chaine de characters
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
 * Conversion de chaine de characters vers valeur numérique
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

template <typename T> Vector2<T> StrToVec2(std::string str, bool withsep = false)
{
    Vector2<T> vec;
    std::stringstream ss(str);

    if(withsep)
    {
        char sep;
        ss >> vec.x >> sep >> vec.y;
    }
    else
    {
        ss >> vec.x >> vec.y;
    }

    return vec;
}

template <typename T> Vector3<T> StrToVec3(std::string str, bool withsep = false)
{
    Vector3<T> vec;
    std::stringstream ss(str);

    if(withsep)
    {
        char sep;
        ss >> vec.x >> sep >> vec.y >> sep >> vec.z;
    }
    else
    {
        ss >> vec.x >> vec.y >> vec.z;
    }

    return vec;
}

template <typename T> Vector4<T> StrToVec4(std::string str, bool withsep = false)
{
    Vector4<T> vec;
    std::stringstream ss(str);

    if(withsep)
    {
        char sep;
        ss >> vec.x >> sep >> vec.y >> sep >> vec.z >> sep >> vec.w;
    }
    else
    {
        ss >> vec.x >> vec.y >> vec.z >> vec.w;
    }

    return vec;
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
 * Renvoi un vectur aléatoir situer a l'interieur de aabb
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
 * Spécialisation de round pour float
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

