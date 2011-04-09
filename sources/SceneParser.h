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

    void loadScene(const std::string& filepath);

    void saveScene();
    void saveScene(const std::string& filepath);

    void setClassFactory(ClassFactory* classFactory);
    ClassFactory* getClassFactory() const;

    MeshParallelScene* getMeshScene() const;
    ParticlesParallelScene* getParticlesScene() const;
    LightParallelScene* getLightScene() const;
    WaterParallelScene* getWaterScene() const;

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

    void parseMap(AttribMap& att);
    void parseFog(AttribMap& att);
    void parseSkyBox(AttribMap& att);
    void parseNode(Relation& att, Node* parent = NULL);

private:
    bool parseBlock(std::ifstream& file, Relation& rel, unsigned& line);

    void outpuNodeConstruction(std::ofstream& file, Node* node);

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

