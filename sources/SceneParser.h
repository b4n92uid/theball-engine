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
#include "MapMarkParalleScene.h"
#include "MapMark.h"

namespace tbe
{
namespace scene
{

class SceneManager;

class SceneParser
{
public:
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

    void setWaterScene(WaterParallelScene* waterScene);
    MeshParallelScene* getMeshScene() const;

    void setParticlesScene(ParticlesParallelScene* particlesScene);
    ParticlesParallelScene* getParticlesScene() const;

    void setMeshScene(MeshParallelScene* meshScene);
    LightParallelScene* getLightScene() const;

    void setLightScene(LightParallelScene* lightScene);
    WaterParallelScene* getWaterScene() const;

    void setMarkScene(MapMarkParalleScene* markScene);
    MapMarkParalleScene* getMarkScene() const;

    SceneParser& exclude(Node* node);

    template<typename T> T getAdditional(std::string key)
    {
        if(!m_additional.count(key))
            return NULL;

        T value;

        std::stringstream ss(m_additional[key]);
        ss >> value;

        return value;
    }

    template<typename T> void setAdditional(std::string key, T value)
    {
        std::stringstream ss;
        ss << value;

        m_additional[key] = ss.str();
    }

    void removeAdditional(std::string key);

    void clearAdditional();

    const AttribMap additionalFields() const;

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

    void setMapDescriptor(MapDescriptor mapDescriptor);
    MapDescriptor getMapDescriptor() const;

protected:
    MapDescriptor m_mapDescriptor;

    AttribMap m_additional;

    SceneManager* m_sceneManager;
    Node* m_rootNode;

private:

    bool parseBlock(std::ifstream& file, Relation& rel, unsigned& line);

    void parseGeneral(AttribMap& att);
    void parseFog(AttribMap& att);
    void parseSkyBox(AttribMap& att);

    void buildNode(Relation& att, Node* parent = NULL);
    void buildMaterial(AttribMap& att, Mesh* mesh);

    void prepareNodeConstruction(Node* node, Relation& rel);
    void outpuNodeConstruction(Relation& rel, std::ofstream& file);

    std::vector<Node*> m_archivedNodes;
    std::vector<Node*> m_excludedNodes;

    LightParallelScene* m_lightScene;
    MeshParallelScene* m_meshScene;
    ParticlesParallelScene* m_particlesScene;
    WaterParallelScene* m_waterScene;
    MapMarkParalleScene* m_markScene;
};

}
}

#endif	/* SCENEPARSER_H */
