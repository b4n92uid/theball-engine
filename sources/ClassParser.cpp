/*
 * File:   ClassParser.cpp
 * Author: b4n92uid
 *
 * Created on 21 mars 2012, 13:15
 */

#include "ClassParser.h"

#include <fstream>

#include "SceneManager.h"
#include "Skybox.h"
#include "Tools.h"
#include "Particles.h"
#include "MapMark.h"
#include "ObjMesh.h"
#include "Ball3DMesh.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

ClassParser::ClassParser() { }

ClassParser::ClassParser(SceneManager* sceneManager) : AbstractParser(sceneManager) { }

ClassParser::~ClassParser() { }

void ClassParser::clear()
{
    AbstractParser::clear();
}

void ClassParser::prepare()
{
    clear();

    // m_buildedNode -> m_scheme
}

void ClassParser::save(const std::string& filepath)
{
    boost::property_tree::write_info(filepath, m_scheme);
}

void ClassParser::load(const std::string& filepath)
{
    clear();

    boost::property_tree::read_info(filepath, m_scheme);

    m_filename = filepath;
}

Node* ClassParser::build()
{
    if(!m_meshScene)
    {
        m_meshScene = new MeshParallelScene;
        m_sceneManager->addParallelScene(m_meshScene);
    }

    if(!m_particlesScene)
    {
        m_particlesScene = new ParticlesParallelScene;
        m_sceneManager->addParallelScene(m_particlesScene);
    }

    if(!m_markScene)
    {
        m_markScene = new MapMarkParallelScene;
        m_sceneManager->addParallelScene(m_markScene);
    }

    return buildNode(m_scheme.get_child("Class"));
}

std::string ClassParser::getClassname() const
{
    return m_scheme.get<string>("Class.name", "noname");
}
