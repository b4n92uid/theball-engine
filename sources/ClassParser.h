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

    void load(const std::string& filepath);
    Node* build();

    void prepare();
    void save(const std::string& filepath);

    std::string getClassname() const;

private:
    rtree m_scheme;
};


}
}

#endif	/* CLASSPARSER_H */
