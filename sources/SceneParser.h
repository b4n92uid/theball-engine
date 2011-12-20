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
#include "MapMarkParallelScene.h"
#include "MapMark.h"

namespace tbe
{
namespace scene
{

class OBJMesh;
class Ball3DMesh;

class SceneManager;

class ParserClassFactory
{
public:
    ParserClassFactory();
    virtual ~ParserClassFactory();

    virtual Mesh* newMesh(MeshParallelScene* scene) = 0;
    virtual Light* newLight(LightParallelScene* scene) = 0;
    virtual ParticlesEmiter* newParticles(ParticlesParallelScene* scene) = 0;
    virtual MapMark* newMapMark(MapMarkParallelScene* scene) = 0;
};

class ParserHandle
{
public:
    ParserHandle();
    ParserHandle(SceneManager* sceneManager);
    virtual ~ParserHandle();

    void setMeshScene(MeshParallelScene* meshScene);
    MeshParallelScene* getMeshScene() const;

    void setParticlesScene(ParticlesParallelScene* particlesScene);
    ParticlesParallelScene* getParticlesScene() const;

    void setLightScene(LightParallelScene* lightScene);
    LightParallelScene* getLightScene() const;

    void setMarkScene(MapMarkParallelScene* markScene);
    MapMarkParallelScene* getMarkScene() const;

    void setClassFactory(ParserClassFactory* classFactory);
    ParserClassFactory* getClassFactory() const;

protected:
    LightParallelScene* m_lightScene;
    MeshParallelScene* m_meshScene;
    ParticlesParallelScene* m_particlesScene;
    MapMarkParallelScene* m_markScene;

    SceneManager* m_sceneManager;

    ParserClassFactory* m_classFactory;

    float m_version;
};

class SceneParser : public ParserHandle
{
public:
    SceneParser();
    SceneParser(SceneManager* sceneManager);
    virtual ~SceneParser();

    typedef std::map<std::string, std::string> AttribMap;

    void setSceneName(std::string sceneName);
    std::string getSceneName() const;

    void setAuthorName(std::string authorName);
    std::string getAuthorName() const;

    void loadScene(const std::string& filepath);
    void buildScene();

    void prepareScene();
    void saveScene();
    void saveScene(const std::string& filepath);

    SceneParser& exclude(Node* node);

    void setAdditionalString(std::string key, std::string value);

    std::string getAdditionalString(std::string key);

    void removeAdditional(std::string key);

    void clearAdditional();

    const AttribMap additionalFields() const;

    template<typename T> T getAdditionalValue(std::string key)
    {
        T value;

        if(!m_additional.count(key))
            return value;

        std::stringstream ss(m_additional[key]);
        ss >> value;

        return value;
    }

    template<typename T> void setAdditionalValue(std::string key, T value)
    {
        std::stringstream ss;
        ss << value;

        m_additional[key] = ss.str();
    }

    struct Relation
    {

        Relation(unsigned deep = 0)
        {
            this->deep = deep;
        }

        AttribMap attr;
        std::vector<Relation> child;
        int deep;
    };

    struct MapDescriptor
    {
        MapDescriptor();

        std::string fileName;
        std::string sceneName;
        std::string authorName;
        Vector4f ambiante;

        float znear, zfar;

        struct
        {
            Vector4f color;
            float start;
            float end;
            bool enable;

        } fog;

        struct
        {
            std::string front, back;
            std::string top, bottom;
            std::string left, right;
            bool enable;

        } skybox;

        std::vector<Relation> nodes;
    };

    MapDescriptor& getMapDescriptor();

protected:
    MapDescriptor m_mapDescriptor;

    AttribMap m_additional;

    Node* m_rootNode;

private:

    bool parseBlock(std::ifstream& file, Relation& rel, unsigned& line);

    void parseGeneral(AttribMap& att);
    void parseFog(AttribMap& att);
    void parseSkyBox(AttribMap& att);

    void buildNode(Relation& att, Node* parent = NULL);
    void buildMaterial(std::string key, std::string value, Mesh* mesh);
    void buildMaterialFromMap(AttribMap& att, Mesh* mesh);
    void buildMaterialFromFile(std::string filepath, Mesh* mesh);

    void prepareNodeConstruction(Node* node, Relation& rel);
    void outpuNodeConstruction(Relation& rel, std::ofstream& file);

    std::vector<Node*> m_archivedNodes;
    std::vector<Node*> m_excludedNodes;
};

class ClassParser : public ParserHandle
{
public:
    ClassParser();
    ClassParser(SceneManager* sceneManager);
    virtual ~ClassParser();

    void loadClass(const std::string& path);
    void buildClass();

    void prepareClass();
    void saveClass();
    void saveClass(const std::string& path);

    void setBuildedNod(Node* buildedNod);
    Node* getBuildedNod() const;

private:
    Node* m_buildedNod;
    std::string m_filename;
};

}
}

#endif	/* SCENEPARSER_H */
