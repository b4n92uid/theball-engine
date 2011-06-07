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

    void loadClass(const std::string& filepath);

    void saveClass();
    void saveClass(const std::string& filepath);

    void loadScene(const std::string& filepath);

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

    SceneParser& archive(Node* node);
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

protected:

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

    void parseGeneral(AttribMap& att);
    void parseFog(AttribMap& att);
    void parseSkyBox(AttribMap& att);
    void parseNode(Relation& att, Node* parent = NULL);

    void parseMaterial(AttribMap& att, Mesh* mesh);

private:

    bool parseBlock(std::ifstream& file, Relation& rel, unsigned& line);
    void outpuNodeConstruction(std::ofstream& file, Node* node);

    std::string m_fileName;
    std::string m_sceneName;
    std::string m_authorName;

    std::vector<Node*> m_archivedNodes;
    std::vector<Node*> m_excludedNodes;

    AttribMap m_additional;

    SceneManager* m_sceneManager;
    Node* m_rootNode;

    LightParallelScene* m_lightScene;
    MeshParallelScene* m_meshScene;
    ParticlesParallelScene* m_particlesScene;
    WaterParallelScene* m_waterScene;

    typedef std::map<std::string, Relation::Child> ClassRec;
    typedef std::vector<std::string> IncludeRec;

    ClassRec m_classRec;
    IncludeRec m_includeRec;
};

}
}

#endif	/* SCENEPARSER_H */
