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

    const rtree& scheme();
    
    rtree& attributes();

private:
    rtree m_scheme;
    rtree m_attributes;
    std::string m_sceneName;
    std::string m_authorName;
};

}
}

#endif	/* SCENEPARSER_H */
