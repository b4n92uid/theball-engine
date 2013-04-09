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
#include "MapMarkParallelScene.h"

#include "AbstractParser.h"

namespace tbe
{
namespace scene
{

class OBJMesh;
class Ball3DMesh;

class SceneManager;

class SceneParser : public AbstractParser
{
public:
    SceneParser();
    SceneParser(SceneManager* sceneManager);
    virtual ~SceneParser();

    void load(const std::string& filepath);
    void build();

    void prepare();
    void save(const std::string& filepath);
    void save();

    void clear();

    void setSceneName(std::string sceneName);
    std::string getSceneName() const;

    void setAuthorName(std::string authorName);
    std::string getAuthorName() const;

    void setAdditionalString(std::string key, std::string value);
    std::string getAdditionalString(std::string key);

    void removeAdditional(std::string key);
    void clearAdditional();

    template<typename T> T getAdditionalValue(std::string key);
    template<typename T> void setAdditionalValue(std::string key, T value);

    const AbstractParser::AttribMap additionalFields() const;

    SceneParser& exclude(Node* node);

    struct MapDescriptor
    {
        MapDescriptor();

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

        struct
        {
            bool enable;
            std::string vert;
            std::string frag;
        } shader;

        std::vector<Relation> nodes;
    };

    MapDescriptor& getMapDescriptor();

protected:
    void prepareNodeConstruction(Node* node, Relation& rel);

    void parseGeneral(AttribMap& att);
    void parseFog(AttribMap& att);
    void parseSkyBox(AttribMap& att);
    void parseShader(AttribMap& att);

    void checkCorrectNode(Relation& rel);

protected:
    MapDescriptor m_mapDescriptor;
    AttribMap m_additional;

private:
    std::vector<Node*> m_archivedNodes;
    std::vector<Node*> m_excludedNodes;
};

template<typename T> T SceneParser::getAdditionalValue(std::string key)
{
    T value;

    if(!m_additional.count(key))
        return value;

    std::stringstream ss(m_additional[key]);
    ss >> value;

    return value;
}

template<typename T> void SceneParser::setAdditionalValue(std::string key, T value)
{
    std::stringstream ss;
    ss << value;

    m_additional[key] = ss.str();
}

}
}

#endif	/* SCENEPARSER_H */
