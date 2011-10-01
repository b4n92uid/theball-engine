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

    unsigned ext = filename.find_last_of('.');

    pos++;

    if(withExt)
        return filename.substr(pos);
    else
        return filename.substr(pos, ext - pos);
}

/**
 * Renvois un tableau contenant les partie découper de str par sep
 *  si le parametre skipempty est a true alors les patie vide seront effacer
 *
 * @param str
 * @param sep
 * @param skipempty
 * @return
 */
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

/**
 * Renvois un tableau contenant les partie de chaine de caracteres
 *  découper de str par sep, si le parametre skipempty est a true
 *  alors les patie vide seront effacer
 *
 * @param str
 * @param sep
 * @param skipempty
 * @return
 */
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

/**
 * Ré-assemble les élement de vec en une chaine de caracteres séparer par glue
 *
 * @param vec
 * @param glue
 * @return
 */
inline std::string joinstr(std::vector<std::string> vec, char glue)
{
    using namespace std;

    string out;

    for(unsigned i = 0; i < vec.size() - 1; i++)
        out += vec[i] + glue;

    out += vec.back();

    return out;
}

#define var_dump(v) tbe::tools::var_dump_print(v, #v, __LINE__, __FILE__)

template<typename T1, typename T2> void var_dump_print(const std::map<T1, T2>& stdmap, const char* name, int line, const char* file)
{
    using namespace std;

    cout << "map(" << name << ")@" << strrchr(file, '/') << ":" << line << endl;

    for(typename map<T1, T2>::const_iterator it = stdmap.begin(); it != stdmap.end(); it++)
        cout << "  [" << it->first << "] = " << it->second << endl;

    cout << endl;
    cout << endl;
}

/**
 * 
 * @param path
 * @return 
 */
inline bool isAbsoloutPath(std::string path)
{
    #ifdef __WIN32__
    return (path.size() >= 1) ? path[1] == ':' : false;
    #else
    return path[0] == '/';
    #endif
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

    for(string::iterator it = buffer.begin(); isspace(*it) && it != buffer.end(); it = buffer.begin())
        buffer.erase(it);

    for(string::reverse_iterator it = buffer.rbegin(); isspace(*it) && it != buffer.rend(); it = buffer.rbegin())
        buffer.erase(it.base());
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

}
}

#endif	/* _TBE_TOOLS_H */
