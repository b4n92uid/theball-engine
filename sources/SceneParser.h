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

    void prepare();
    void build();

    void load(const std::string& filepath);
    void save(const std::string& filepath);
    void save();

    void clear();

    void setSceneName(std::string sceneName);
    std::string getSceneName() const;

    void setAuthorName(std::string authorName);
    std::string getAuthorName() const;

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

        std::vector<Relation> nodes;
    };

    MapDescriptor& getMapDescriptor();

protected:
    Node* buildNode(Relation& att, Node* parent = NULL);

    void prepareNodeConstruction(Node* node, Relation& rel);

    void parseGeneral(AttribMap& att);
    void parseFog(AttribMap& att);
    void parseSkyBox(AttribMap& att);
    
    void checkCorrectNode(Relation& rel);

protected:
    MapDescriptor m_mapDescriptor;

private:
    std::vector<Node*> m_archivedNodes;
    std::vector<Node*> m_excludedNodes;
};

}
}

#endif	/* SCENEPARSER_H */
