/* 
 * File:   SceneParser.h
 * Author: b4n92uid
 *
 * Created on 5 novembre 2010, 14:19
 */

#ifndef SCENEPARSER_H
#define	SCENEPARSER_H

#include <string>
#include <map>

#include "MeshParallelScene.h"
#include "ParticlesParallelScene.h"

namespace tbe
{
namespace scene
{

class SceneManager;

class ClassFactory
{
public:
    virtual Mesh* Instance(std::string type) = 0;
};

class SceneParser
{
public:
    SceneParser(SceneManager* sceneManager);
    virtual ~SceneParser();

    void LoadScene(const std::string& filepath);

    void SetClassFactory(ClassFactory* classFactory);
    ClassFactory* GetClassFactory() const;

protected:

    typedef std::map<std::string, std::string> AttribMap;
    typedef std::vector<AttribMap> AttribMapArray;

    void ParseMap(AttribMap& att);
    void ParseFog(AttribMap& att);
    void ParseSkyBox(AttribMap& att);
    void ParseLight(AttribMap& att);
    void ParseNode(AttribMap& att, Mesh* parent = NULL);
    void ParseParticles(AttribMap& att);

    void RecordClass(std::ifstream& file, std::string type);

    AttribMap GetAttributs(std::ifstream& file);

private:

    typedef std::map<std::string, AttribMapArray> ClassRec;

    std::string m_mapName;

    SceneManager* m_sceneManager;
    MeshParallelScene* m_meshScene;
    ParticlesParallelScene* m_particleScene;

    ClassRec m_classRec;

    ClassFactory* m_classFactory;
};

}
}

#endif	/* SCENEPARSER_H */

