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

#include "MeshParallelScene.h"
#include "ParticlesParallelScene.h"

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

    void LoadScene(const std::string& filepath);

protected:

    typedef std::map<std::string, std::string> AttribMap;

    void ParseMap(AttribMap& att);
    void ParseFog(AttribMap& att);
    void ParseSkyBox(AttribMap& att);
    void ParseLight(AttribMap& att);
    void ParseNode(AttribMap& att, Mesh* parent = NULL);
    void ParseParticles(AttribMap& att);
    
    void RecordClass(std::ifstream& file, std::string name);

    AttribMap GetAttributs(std::ifstream& file);

private:

    typedef std::map<std::string, Mesh*> ClassRec;

    std::string m_mapName;

    SceneManager* m_sceneManager;
    MeshParallelScene* m_meshScene;
    ParticlesParallelScene* m_particleScene;
    
    ClassRec m_classRec;
};

}
}

#endif	/* BLDPARSER_H */

