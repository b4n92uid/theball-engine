/* 
 * File:   ClassParser.h
 * Author: b4n92uid
 *
 * Created on 21 mars 2012, 13:15
 */

#ifndef CLASSPARSER_H
#define	CLASSPARSER_H

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

class ClassParser : public AbstractParser
{
public:
    ClassParser();
    ClassParser(SceneManager* sceneManager);
    virtual ~ClassParser();

    void clear();

    void save(const std::string& filepath);
    void load(const std::string& filepath);

    void prepare();
    void build();

    std::string getAuthorname() const;
    std::string getClassname() const;

    void setBuildedNode(Node* buildedNod);
    Node* getBuildedNode() const;

    template<typename T> T getBuildedNode();

private:
    Node* m_buildedNode;
    std::string m_classname;
    std::string m_authorname;
    Relation m_scheme;
};

template<typename T> T ClassParser::getBuildedNode()
{
    return dynamic_cast<T> (getBuildedNode());
}

}
}

#endif	/* CLASSPARSER_H */
