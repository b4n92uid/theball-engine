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
    virtual Light* newLight(MeshParallelScene* scene) = 0;
    virtual ParticlesEmiter* newParticles(ParticlesParallelScene* scene) = 0;
    virtual MapMark* newMapMark(MapMarkParallelScene* scene) = 0;

    virtual void setupMesh(Mesh* mesh) { }

    virtual void setupLight(Light* light) { }

    virtual void setupParticles(ParticlesEmiter* particles) { }

    virtual void setupMapMark(MapMark* mapmark) { }
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

protected:
    bool parseBlock(std::ifstream& file, Relation& rel);

    Node* buildNode(Relation& att, Node* parent = NULL);
    void buildMaterial(std::string key, std::string value, Mesh* mesh);
    void buildMaterial(AttribMap attr, Mesh* mesh);

    void outpuNodeConstruction(Relation& rel, std::ofstream& file);

protected:
    MeshParallelScene* m_meshScene;
    ParticlesParallelScene* m_particlesScene;
    MapMarkParallelScene* m_markScene;

    SceneManager* m_sceneManager;
    Node* m_rootNode;

    ParserClassFactory* m_classFactory;

    float m_version;
    unsigned m_parseLine;

    std::string m_filename;
};

}
}

#endif	/* ABSTRACTPARSER_H */
