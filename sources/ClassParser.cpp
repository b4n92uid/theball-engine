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

ClassParser::ClassParser()
{
    m_buildedNode = NULL;
}

ClassParser::ClassParser(SceneManager* sceneManager) : AbstractParser(sceneManager)
{
    m_buildedNode = NULL;
}

ClassParser::~ClassParser()
{
}

void ClassParser::clear()
{
    AbstractParser::clear();

    delete m_buildedNode, m_buildedNode = NULL;

    m_scheme.attr.clear();
    m_scheme.child.clear();
    m_scheme.deep = 0;

    m_classname.clear();
    m_authorname.clear();
}

void ClassParser::prepare()
{
}

void ClassParser::save(const std::string& filepath)
{
    ofstream file(filepath.c_str());

    if(!file)
        throw tbe::Exception("ClassParser::saveScene; Open file error (%s)", filepath.c_str());

    file << "v0.1 class" << endl;
    file << endl;

    file << "*general" << endl;
    file << "name=" << m_classname << endl;
    file << "author=" << m_authorname << endl;
    file << endl;

    m_scheme.attr = m_buildedNode->constructionMap(filepath);

    outpuNodeConstruction(m_scheme, file);

    file.close();
}

void ClassParser::load(const std::string& filepath)
{
    ifstream file(filepath.c_str());

    if(!file)
        throw tbe::Exception("ClassParser::loadClass; Open file error (%s)", filepath.c_str());

    clear();

    m_filename = filepath;

    string buffer;

    tools::getline(file, buffer);

    for(unsigned i = 0; i < buffer.size(); i++)
        if(buffer[i] <= 0) throw tbe::Exception("ClassParser::loadClass; File is binary; (%s)", filepath.c_str());

    if(buffer[0] == 'v')
    {
        m_version = tools::strToNum<float>(&buffer[1]);
    }
    else
        file.seekg(0);

    while(tools::getline(file, buffer))
    {
        if(buffer.empty() || buffer[0] == '#')
            continue;

        if(buffer == "*general")
        {
            Relation rel;
            parseBlock(file, rel);

            m_classname = rel.attr["name"];
            m_authorname = rel.attr["author"];
        }

        else if(buffer == "*class")
        {
            parseBlock(file, m_scheme);
        }

        else
            throw tbe::Exception("ClassParser::loadClass; Parse error (%s:%d)", buffer.c_str(), m_parseLine);
    }

    file.close();
}

void ClassParser::build()
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

    m_buildedNode = buildNode(m_scheme);
}

std::string ClassParser::getAuthorname() const
{
    return m_authorname;
}

std::string ClassParser::getClassname() const
{
    return m_classname;
}

void ClassParser::setBuildedNode(Node* buildedNod)
{
    this->m_buildedNode = buildedNod;
}

Node* ClassParser::getBuildedNode() const
{
    return m_buildedNode;
}
