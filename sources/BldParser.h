/* 
 * File:   BldParser.h
 * Author: b4n92uid
 *
 * Created on 5 novembre 2010, 14:19
 */

#ifndef BLDPARSER_H
#define	BLDPARSER_H

#include <string>
#include <map>

namespace tbe
{
namespace scene
{

class SceneManager;

class BldParser
{
public:
    BldParser(SceneManager* sceneManager);
    virtual ~BldParser();

    void LoadScene(std::string filepath);

protected:

    typedef std::map<std::string, std::string> AttribMap;

    void ParseMap(AttribMap& att);
    void ParseFog(AttribMap& att);
    void ParseSkyBox(AttribMap& att);
    void ParseLight(AttribMap& att);
    void ParseNode(AttribMap& att);
    void ParseParticles(AttribMap& att);
    void ParseClass(AttribMap& att);

    AttribMap GetAttributs(std::ifstream& file);

private:
    SceneManager* m_sceneManager;
    std::string m_mapName;
};

}
}

#endif	/* BLDPARSER_H */

