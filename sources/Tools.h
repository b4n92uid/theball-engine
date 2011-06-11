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
#include <vector>

#include "Mathematics.h"
#include "AABB.h"
#include "Exception.h"

namespace tbe
{
namespace tools
{

inline bool isEqual(float a, float b)
{
    return (fabs(a - b) < 0.0001);
}

inline bool isEqual(Vector3f a, float b)
{
    return (isEqual(a.x, b) && isEqual(a.y, b) && isEqual(a.z, b));
}

inline bool isZero(Vector3f a)
{
    return (a.x < 0.0001 && a.x > 0.0001
            && a.y < 0.0001 && a.y > 0.0001
            && a.z < 0.0001 && a.z > 0.0001);
}

inline bool isZero(float a)
{
    return (a < 0.0001 && a > 0.0001);
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
 * Renvois le chemin avec les separateur en slash
 *
 * @param path
 * @return
 */
inline std::string toSlashSeprator(std::string path)
{
    unsigned reppos;
    while((reppos = path.find('\\')) != std::string::npos)
        path.replace(reppos, 1, 1, '/');

    return path;
}

/**
 * Renvois le chemin du fichier, sans le separateur de fin
 *
 * @param filename
 * @return
 */
inline std::string pathname(std::string filename)
{
    unsigned pos = filename.find_last_of('\\');

    if(pos == std::string::npos)
        pos = filename.find_last_of('/');

    return filename.substr(0, pos);
}

/**
 * Renvois le nom (et l'extention si withExt est à true)
 *  du chemin spécifier par filename
 *
 * @param filename
 * @param withExt
 * @return
 */
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

template<typename T> std::vector<T> tokenize(std::string str, char sep, bool skipempty = false)
{
    std::vector<T> tokens;

    unsigned lastoffet = 0;
    for(unsigned i = 0; i < str.size(); i++)
        if(str[i] == sep)
        {
            if(i == lastoffet)
                if(skipempty)
                    continue;
                else
                    tokens.push_back(T());

            else
            {
                T value;

                std::stringstream ss(str.substr(lastoffet, i - lastoffet));
                ss >> value;

                tokens.push_back(value);
                lastoffet = i + 1;
            }
        }

    tokens.push_back(str.substr(lastoffet));

    return tokens;
}

inline std::vector<std::string> tokenize(std::string str, char sep, bool skipempty = false)
{
    using namespace std;

    vector<string> tokens;

    unsigned lastoffet = 0;

    for(unsigned i = 0; i < str.size(); i++)
        if(str[i] == sep)
        {
            if(i == lastoffet)
                if(skipempty)
                    continue;
                else
                    tokens.push_back(string());

            else
                tokens.push_back(str.substr(lastoffet, i - lastoffet));

            lastoffet = i + 1;
        }

    tokens.push_back(str.substr(lastoffet));

    return tokens;
}

inline std::string joinstr(std::vector<std::string> vec, char glue)
{
    using namespace std;

    string out;

    for(unsigned i = 0; i < vec.size() - 1; i++)
        out += vec[i] + glue;

    out += vec.back();

    return out;
}

/**
 * Renvois le chemin du fichier 'relfile' relatif a au chemin
 *  du fichier 'absfile'
 *
 * @param absfile
 * @param relfile
 * @return
 */
inline std::string pathScope(std::string absfile, std::string relfile, bool absoluteOut)
{
    using namespace std;

    absfile = toSlashSeprator(absfile);
    relfile = toSlashSeprator(relfile);

    vector<string> abscomp;
    vector<string> relcomp;

    unsigned sc = 0;

    for(unsigned i = 0; i < absfile.size(); i++)
    {
        if(absfile[i] == '/')
        {
            abscomp.push_back(absfile.substr(sc, i - sc));
            sc = i + 1;
        }
    }

    abscomp.push_back(absfile.substr(sc));

    sc = 0;

    for(unsigned i = 0; i < relfile.size(); i++)
    {
        if(relfile[i] == '/')
        {
            relcomp.push_back(relfile.substr(sc, i - sc));
            sc = i + 1;
        }
    }

    relcomp.push_back(relfile.substr(sc));

    abscomp.pop_back();

    bool relisabs =
            #if __WIN32__
            relcomp.front()[1] == ':';
            #elif __linux__
            relfile[0] == '/';
    #endif

    string out;

    if(absoluteOut)
    {
        while(!relcomp.empty())
            if(relcomp.front() == "..")
            {
                abscomp.pop_back();
                relcomp.erase(relcomp.begin());
            }
            else
                break;

        if(relisabs)
            out = relfile;
        else
            out = joinstr(abscomp, '/') + '/' + joinstr(relcomp, '/');
    }
    else
    {
        bool done = false;

        for(unsigned i = 0; i < abscomp.size(); i++)
        {
            if(abscomp[i] != relcomp[i])
            {
                relcomp.erase(relcomp.begin(), relcomp.begin() + i);
                if(relisabs)
                    relcomp.insert(relcomp.begin(), abscomp.size() - i, "..");
                done = true;
                break;
            }
        }

        if(!done)
            relcomp.erase(relcomp.begin(), relcomp.begin() + abscomp.size());

        out = joinstr(relcomp, '/');
    }

    return out;
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

template<typename T> void erase(std::vector<T>& vec, unsigned index)
{
    vec.erase(vec.begin() + index);
}

/**
 * Trouve et efface la premiere occurance de 'val'
 *  si elle est contenue dans le tableau 'vec',
 *  val peut être d'un type héritant de T
 *
 * @param vec
 * @param val
 */
template<typename T, typename T2> void erase(std::vector<T>& vec, T2 val)
{
    typename std::vector<T>::iterator it = std::find(vec.begin(), vec.end(), dynamic_cast<T2>(val));

    if(it == vec.end())
        throw Exception("tools::erase<T, T2>; value not found %p", val);

    vec.erase(it);
}

/**
 * Trouve et efface la premiere occurance de 'val'
 *  si elle est contenue dans le tableau 'vec'
 *
 * @param vec
 * @param val
 */
template<typename T> void erase(std::vector<T>& vec, T val)
{
    typename std::vector<T>::iterator it = std::find(vec.begin(), vec.end(), val);

    if(it == vec.end())
        throw Exception("tools::erase<T>; value not found %p", val);

    vec.erase(it);
}

/**
 * Indique si la valeur 'val' est contenue dans le tableau 'vec',
 *  val peut être d'un type héritant de T
 *
 * @param vec
 * @param val
 * @return
 */
template<typename T, typename T2> T find(const std::vector<T>& vec, T2 val)
{
    typename std::vector<T>::const_iterator it = std::find(vec.begin(), vec.end(), val);
    return (it != vec.end()) ? *it : NULL;
}

/**
 * Indique si la valeur 'val' est contenue dans le tableau 'vec'
 *
 * @param vec
 * @param val
 * @return
 */
template<typename T> bool find(const std::vector<T>& vec, T val)
{
    return std::find(vec.begin(), vec.end(), val) != vec.end();
}

/**
 * Enleve les espace blanc des extrémité de la chaine
 *
 * @param buffer
 */
inline void trimstr(std::string& buffer)
{
    using namespace std;

    for(string::iterator it = buffer.begin(); isspace(*it); it = buffer.begin())
        buffer.erase(it);

    for(string::iterator it = --buffer.end(); isspace(*it); it--)
        buffer.erase(it);
}

/**
 * Renvois 'true' si la chaine de caracter ne contien que des espaces
 *
 * @param buffer
 * @return
 */
inline bool isspace(const std::string& buffer)
{
    for(unsigned i = 0; i < buffer.size(); i++)
        if(!std::isspace(buffer[i]))
            return false;

    return true;
}

/**
 * Surcharge de la fonction standard getline, efface le retour a la ligne
 *  sur les systeme Unix
 *
 * @param stream
 * @param buffer
 * @return
 */
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
 * Retourne la chaine 'name' sous format de nom compatible Unix
 *
 * @param name
 * @return
 */
inline std::string unixName(std::string name)
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
std::string nameGen(std::map<std::string, T2>& map, std::string prefix = std::string())
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
std::string nameGen(std::vector<T>& array, std::string prefix = std::string())
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
 * Conversion numérique vers chaine de characters
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
template <typename T> T strToNum(std::string str)
{
    T numeric;
    std::stringstream ss(str);
    ss >> numeric;
    return numeric;
}

/**
 * Interprétation d'une chaine de caracteres en vecteur a 2 composant
 *  depuis le format "x, y"
 *
 * @param str
 * @param withsep
 * @return
 */
template <typename T> Vector2<T> strToVec2(std::string str, bool withsep = false)
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

/**
 * Interprétation d'une chaine de caracteres en vecteur a 3 composant
 *  depuis le format "x, y, z"
 *
 * @param str
 * @param withsep
 * @return
 */
template <typename T> Vector3<T> strToVec3(std::string str, bool withsep = false)
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

/**
 * Interprétation d'une chaine de caracteres en vecteur a 4 composant
 *  depuis le format "x, y, z, w"
 *
 * @param str
 * @param withsep
 * @return
 */
template <typename T> Vector4<T> strToVec4(std::string str, bool withsep = false)
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
 * Conversion d'une matrice 4x4 en chaine de caracteres
 *  au format "0.1, 0.2, 0.3[...]"
 *
 * @param mat
 * @return
 */
inline std::string mat4ToStr(const Matrix4& mat)
{
    std::stringstream stream;
    for(unsigned i = 0; i < 15; i++)
        stream << mat[i] << ",";

    stream << mat[15];

    return stream.str();
}

/**
 * Conversion d'un vecteur a 2 composant en chaine de caracteres
 *  au format "x, y"
 *
 * @param vec
 * @return
 */
template<typename T> inline std::string vec2ToStr(const Vector2<T>& vec)
{
    std::stringstream stream;
    stream << vec.x << ", " << vec.y;

    return stream.str();
}

/**
 * Conversion d'un vecteur a " composant en chaine de caracteres
 *  au format "x, y, z"
 *
 * @param vec
 * @return
 */
template<typename T> inline std::string vec3ToStr(const Vector3<T>& vec)
{
    std::stringstream stream;
    stream << vec.x << ", " << vec.y << ", " << vec.z;

    return stream.str();
}

/**
 * Conversion d'un vecteur a 4 composant en chaine de caracteres
 *  au format "x, y, z, w"
 *
 * @param vec
 * @return
 */
template<typename T> inline std::string vec4ToStr(const Vector4<T>& vec)
{
    std::stringstream stream;
    stream << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w;

    return stream.str();
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
 * Renvoi un vectur aléatoir situer a l'interieur de la boite englobant 'aabb'
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
    tools::round(value.x, unit.x);
    tools::round(value.y, unit.y);
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
    tools::round(value.x, unit.x);
    tools::round(value.y, unit.y);
    tools::round(value.z, unit.z);
}

}
}
#endif	/* _TBE_TOOLS_H */

