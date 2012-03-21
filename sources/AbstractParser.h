/* 
 * File:   AbstractParser.h
 * Author: b4n92uid
 *
 * Created on 21 mars 2012, 13:11
 */

#ifndef ABSTRACTPARSER_H
#define	ABSTRACTPARSER_H

#include <string>
#include <map>

#include "LightParallelScene.h"
#include "MeshParallelScene.h"
#include "ParticlesParallelScene.h"
#include "MapMarkParallelScene.h"

namespace tbe
{
namespace scene
{

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

class AbstractParser
{
public:
    AbstractParser();
    AbstractParser(SceneManager* sceneManager);
    virtual ~AbstractParser();

    virtual void prepare() = 0;
    virtual void build() = 0;

    virtual void load(const std::string& filepath) = 0;
    virtual void save(const std::string& filepath) = 0;

    virtual void clear();

    std::string getFilename() const;

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

    typedef std::map<std::string, std::string> AttribMap;

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

    void setAdditionalString(std::string key, std::string value);
    std::string getAdditionalString(std::string key);

    void removeAdditional(std::string key);
    void clearAdditional();

    const AbstractParser::AttribMap additionalFields() const;

    template<typename T> T getAdditionalValue(std::string key);
    template<typename T> void setAdditionalValue(std::string key, T value);

protected:
    bool parseBlock(std::ifstream& file, Relation& rel);

    virtual Node* buildNode(Relation& att, Node* parent = NULL);
    void buildMaterial(std::string key, std::string value, Mesh* mesh);
    void buildMaterial(AttribMap attr, Mesh* mesh);

    void outpuNodeConstruction(Relation& rel, std::ofstream& file);

protected:
    LightParallelScene* m_lightScene;
    MeshParallelScene* m_meshScene;
    ParticlesParallelScene* m_particlesScene;
    MapMarkParallelScene* m_markScene;

    SceneManager* m_sceneManager;
    Node* m_rootNode;

    ParserClassFactory* m_classFactory;

    AttribMap m_additional;

    float m_version;
    unsigned m_parseLine;

    std::string m_filename;
};

template<typename T> T AbstractParser::getAdditionalValue(std::string key)
{
    T value;

    if(!m_additional.count(key))
        return value;

    std::stringstream ss(m_additional[key]);
    ss >> value;

    return value;
}

template<typename T> void AbstractParser::setAdditionalValue(std::string key, T value)
{
    std::stringstream ss;
    ss << value;

    m_additional[key] = ss.str();
}

}
}

#endif	/* ABSTRACTPARSER_H */
