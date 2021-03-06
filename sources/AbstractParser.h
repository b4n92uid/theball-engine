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
#include "WaterParallelScene.h"

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/fusion/iterator/advance.hpp>
#include <boost/algorithm/string.hpp>

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

    virtual void clear();

    void setFilename(std::string filename);
    std::string getFilename() const;

    void setMeshScene(MeshParallelScene* meshScene);
    MeshParallelScene* getMeshScene() const;

    void setParticlesScene(ParticlesParallelScene* particlesScene);
    ParticlesParallelScene* getParticlesScene() const;

    void setMarkScene(MapMarkParallelScene* markScene);
    MapMarkParallelScene* getMarkScene() const;

    void setWaterScene(WaterParallelScene* waterScene);
    WaterParallelScene* getWaterScene() const;

    void setClassFactory(ParserClassFactory* classFactory);
    ParserClassFactory* getClassFactory() const;

    std::string resolve(std::string relpath, std::string base = "");
    std::string relativize(std::string abspath);

protected:
    Node* buildNode(rtree data, Node* parent = NULL);
    void buildInherited(rtree data, Node* parent, Node* current);

protected:
    MeshParallelScene* m_meshScene;
    ParticlesParallelScene* m_particlesScene;
    MapMarkParallelScene* m_markScene;
    WaterParallelScene* m_waterScene;

    SceneManager* m_sceneManager;
    Node* m_rootNode;

    ParserClassFactory* m_classFactory;

    std::string m_filename;
};

struct TextureTranslator
{
    typedef std::string internal_type;
    typedef Texture external_type;

    // Converts a string to bool

    boost::optional<external_type> get_value(const internal_type& str)
    {
        if(!str.empty())
        {
            return Texture(str, true);
        }
        else
            return boost::optional<external_type>(boost::none);
    }

    // Converts a bool to string

    boost::optional<internal_type> put_value(const external_type& b)
    {
        return boost::optional<internal_type>(b.getFilename());
    }
};

}
}

#endif	/* ABSTRACTPARSER_H */
