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

#include "LightParallelScene.h"
#include "MeshParallelScene.h"
#include "ParticlesParallelScene.h"
#include "WaterParallelScene.h"

namespace tbe
{
namespace scene
{

class SceneManager;

class ClassFactory
{
public:
    virtual Mesh* Instance(std::string type, MeshParallelScene* meshScene) = 0;
};

class SceneParser
{
public:
    SceneParser(SceneManager* sceneManager);
    virtual ~SceneParser();

    void LoadScene(const std::string& filepath);

    void SaveScene();
    void SaveScene(const std::string& filepath);

    void SetClassFactory(ClassFactory* classFactory);
    ClassFactory* GetClassFactory() const;

    MeshParallelScene* GetMeshScene() const;
    ParticlesParallelScene* GetParticlesScene() const;
    LightParallelScene* GetLightScene() const;
    WaterParallelScene* GetWaterScene() const;

protected:

    typedef std::map<std::string, std::string> AttribMap;

    struct Relation
    {

        Relation(unsigned deep = 0)
        {
            this->deep = deep;
        }

        typedef std::vector<Relation> Child;

        AttribMap attr;
        Child child;
        int deep;
    };

    void ParseMap(AttribMap& att);
    void ParseFog(AttribMap& att);
    void ParseSkyBox(AttribMap& att);
    void ParseNode(Relation& att, Node* parent = NULL);

private:
    bool parseBlock(std::ifstream& file, Relation& rel, unsigned& line);

    void OutpuNodeConstruction(std::ofstream& file, Node* node);

    std::string m_fileName;

    std::string m_mapName;

    SceneManager* m_sceneManager;
    Node* m_rootNode;

    LightParallelScene* m_lightScene;
    MeshParallelScene* m_meshScene;
    ParticlesParallelScene* m_particlesScene;
    WaterParallelScene* m_waterScene;

    typedef std::map<std::string, Relation::Child> ClassRec;

    ClassRec m_classRec;

    ClassFactory* m_classFactory;
};

}
}

#endif	/* SCENEPARSER_H */

