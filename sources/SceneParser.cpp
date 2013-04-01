/*
 * File:   SceneParser.cpp
 * Author: b4n92uid
 *
 * Created on 5 novembre 2010, 14:19
 */

#include "SceneParser.h"

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

SceneParser::SceneParser()
{
}

SceneParser::SceneParser(SceneManager* sceneManager) : AbstractParser(sceneManager)
{
}

SceneParser::~SceneParser()
{
}

SceneParser& SceneParser::exclude(Node *node)
{
    m_excludedNodes.push_back(node);
    return *this;
}

void SceneParser::prepare()
{
    Fog* fog = m_sceneManager->getFog();

    m_mapDescriptor.fog.enable = fog->isEnable();
    m_mapDescriptor.fog.color = fog->getColor();
    m_mapDescriptor.fog.end = fog->getEnd();
    m_mapDescriptor.fog.start = fog->getStart();

    SkyBox* sky = m_sceneManager->getSkybox();
    Texture* skytex = sky->getTextures();

    m_mapDescriptor.skybox.enable = sky->isEnable();
    m_mapDescriptor.skybox.front = tools::pathScope(m_filename, skytex[0].getFilename(), false);
    m_mapDescriptor.skybox.back = tools::pathScope(m_filename, skytex[1].getFilename(), false);
    m_mapDescriptor.skybox.top = tools::pathScope(m_filename, skytex[2].getFilename(), false);
    m_mapDescriptor.skybox.bottom = tools::pathScope(m_filename, skytex[3].getFilename(), false);
    m_mapDescriptor.skybox.left = tools::pathScope(m_filename, skytex[4].getFilename(), false);
    m_mapDescriptor.skybox.right = tools::pathScope(m_filename, skytex[5].getFilename(), false);

    Shader rshade = m_meshScene->getRenderingShader();

    m_mapDescriptor.shader.enable = (bool)rshade;
    m_mapDescriptor.shader.vert = tools::pathScope(m_filename, rshade.getVertFilename(), false);
    m_mapDescriptor.shader.frag = tools::pathScope(m_filename, rshade.getFragFilename(), false);

    m_mapDescriptor.ambiante = m_sceneManager->getAmbientLight();
    m_mapDescriptor.znear = m_sceneManager->getZNear();
    m_mapDescriptor.zfar = m_sceneManager->getZFar();

    m_mapDescriptor.nodes.clear();

    for(Iterator<Node*> it = m_rootNode->getChildIterator(); it; it++)
    {
        if(tools::find(m_excludedNodes, *it))
            continue;

        Relation rel;
        prepareNodeConstruction(*it, rel);
        m_mapDescriptor.nodes.push_back(rel);
    }
}

void SceneParser::save()
{
    if(m_filename.empty())
        return;

    save(m_filename);
}

void SceneParser::save(const std::string& filepath)
{
    ofstream file(filepath.c_str());

    if(!file)
        throw tbe::Exception("SceneParser::SaveScene; Open file error (%s)", filepath.c_str());

    m_filename = filepath;

    file << "v0.1 scene" << endl;
    file << endl;

    file << "*general" << endl;
    file << "name=" << m_mapDescriptor.sceneName << endl;
    file << "author=" << m_mapDescriptor.authorName << endl;
    file << "ambient=" << m_mapDescriptor.ambiante << endl;
    file << "znear=" << m_mapDescriptor.znear << endl;
    file << "zfar=" << m_mapDescriptor.zfar << endl;
    file << endl;

    if(m_additional.size())
    {
        file << "*additional" << endl;
        for(AttribMap::iterator it = m_additional.begin(); it != m_additional.end(); it++)
            file << it->first << "=" << it->second << endl;
        file << endl;
    }

    if(m_mapDescriptor.fog.enable)
    {
        file << "*fog" << endl;
        file << "color=" << m_mapDescriptor.fog.color << endl;
        file << "start=" << m_mapDescriptor.fog.start << endl;
        file << "end=" << m_mapDescriptor.fog.end << endl;
        file << endl;
    }

    if(m_mapDescriptor.skybox.enable)
    {
        file << "*skybox" << endl;
        file << "front=" << m_mapDescriptor.skybox.front << endl;
        file << "back=" << m_mapDescriptor.skybox.back << endl;
        file << "top=" << m_mapDescriptor.skybox.top << endl;
        file << "bottom=" << m_mapDescriptor.skybox.bottom << endl;
        file << "left=" << m_mapDescriptor.skybox.left << endl;
        file << "right=" << m_mapDescriptor.skybox.right << endl;
        file << endl;
    }

    if(m_mapDescriptor.shader.enable)
    {
        file << "*shader" << endl;
        file << "vertex=" << m_mapDescriptor.shader.vert << endl;
        file << "fragment=" << m_mapDescriptor.shader.frag << endl;
        file << endl;
    }

    for(unsigned i = 0; i < m_mapDescriptor.nodes.size(); i++)
        outpuNodeConstruction(m_mapDescriptor.nodes[i], file);

    file.close();
}

void SceneParser::clear()
{
    AbstractParser::clear();

    m_archivedNodes.clear();
    m_excludedNodes.clear();

    m_mapDescriptor.sceneName.clear();
    m_mapDescriptor.authorName.clear();
    m_mapDescriptor.ambiante = 0.2;
    m_mapDescriptor.znear = 0.1;
    m_mapDescriptor.zfar = 512;
    m_mapDescriptor.fog.color = 0;
    m_mapDescriptor.fog.start = 32;
    m_mapDescriptor.fog.end = 64;
    m_mapDescriptor.fog.enable = false;
    m_mapDescriptor.skybox.top.clear();
    m_mapDescriptor.skybox.front.clear();
    m_mapDescriptor.skybox.back.clear();
    m_mapDescriptor.skybox.top.clear();
    m_mapDescriptor.skybox.bottom.clear();
    m_mapDescriptor.skybox.left.clear();
    m_mapDescriptor.skybox.right.clear();
    m_mapDescriptor.skybox.enable = false;
    m_mapDescriptor.shader.vert.clear();
    m_mapDescriptor.shader.frag.clear();
    m_mapDescriptor.shader.enable = false;
    m_mapDescriptor.nodes.clear();
}

void SceneParser::checkCorrectNode(Relation& rel)
{
    if(!rel.attr.count("class"))
        throw tbe::Exception("SceneParser::checkCorrectNode; Class undefined (%s:%d)", m_filename.c_str(), m_parseLine);

    if(rel.attr["class"].empty())
        throw tbe::Exception("SceneParser::checkCorrectNode; Class empty (%s:%d)", m_filename.c_str(), m_parseLine);

    if(!rel.attr.count("name"))
        throw tbe::Exception("SceneParser::checkCorrectNode; Name undefined (%s:%d)", m_filename.c_str(), m_parseLine);
}

void SceneParser::load(const std::string& filepath)
{
    ifstream file(filepath.c_str());

    if(!file)
        throw tbe::Exception("SceneParser::loadScene; Open file error (%s)", filepath.c_str());

    clear();

    m_filename = filepath;

    string buffer;

    tools::getline(file, buffer);

    for(unsigned i = 0; i < buffer.size(); i++)
        if(buffer[i] <= 0) throw tbe::Exception("SceneParser::loadScene; File is binary; (%s)", filepath.c_str());

    if(buffer[0] == 'v')
    {
        m_version = tools::strToNum<float>(&buffer[1]);

        /*
        int pos = buffer.find(' ');

        if(pos != string::npos)
            buffer[pos + 1];
         */
    }
    else
        file.seekg(0);

    for(m_parseLine = 0; tools::getline(file, buffer); m_parseLine++)
    {
        if(buffer.empty() || buffer[0] == '#')
            continue;

        if(buffer == "*general")
        {
            Relation rel;
            parseBlock(file, rel);
            parseGeneral(rel.attr);
        }

        else if(buffer == "*additional")
        {
            Relation rel;
            parseBlock(file, rel);
            m_additional = rel.attr;
        }

        else if(buffer == "*fog")
        {
            Relation rel;
            parseBlock(file, rel);
            parseFog(rel.attr);
        }

        else if(buffer == "*skybox")
        {
            Relation rel;
            parseBlock(file, rel);
            parseSkyBox(rel.attr);
        }

        else if(buffer == "*shader")
        {
            Relation rel;
            parseBlock(file, rel);
            parseShader(rel.attr);
        }

        else if(buffer == "+node")
        {
            Relation rel;
            parseBlock(file, rel);
            checkCorrectNode(rel);
            m_mapDescriptor.nodes.push_back(rel);
        }

        else if(buffer.substr(0, 8) == ".include")
        {
            string modelFilepath(buffer, 9);

            if(modelFilepath.find(':') == string::npos)
                modelFilepath = tools::pathScope(m_filename, modelFilepath, true);

            load(modelFilepath);
        }

        else
            throw tbe::Exception("SceneParser::LoadScene; Parse error (%s:%d)", buffer.c_str(), m_parseLine);
    }

    file.close();
}

void SceneParser::parseFog(AttribMap& att)
{
    m_mapDescriptor.fog.color.fromStr(att["color"]);
    m_mapDescriptor.fog.start = tools::strToNum<float>(att["start"]);
    m_mapDescriptor.fog.end = tools::strToNum<float>(att["end"]);
    m_mapDescriptor.fog.enable = true;
}

void SceneParser::parseSkyBox(AttribMap& att)
{
    if(!att["front"].empty())
        m_mapDescriptor.skybox.front = tools::pathScope(m_filename, att["front"], true);

    if(!att["back"].empty())
        m_mapDescriptor.skybox.back = tools::pathScope(m_filename, att["back"], true);

    if(!att["top"].empty())
        m_mapDescriptor.skybox.top = tools::pathScope(m_filename, att["top"], true);

    if(!att["bottom"].empty())
        m_mapDescriptor.skybox.bottom = tools::pathScope(m_filename, att["bottom"], true);

    if(!att["left"].empty())
        m_mapDescriptor.skybox.left = tools::pathScope(m_filename, att["left"], true);

    if(!att["right"].empty())
        m_mapDescriptor.skybox.right = tools::pathScope(m_filename, att["right"], true);

    m_mapDescriptor.skybox.enable = true;
}

void SceneParser::parseShader(AttribMap& att)
{
    if(att.count("vertex"))
        m_mapDescriptor.shader.vert = att["vertex"];

    if(att.count("fragment"))
        m_mapDescriptor.shader.frag = att["fragment"];

    m_mapDescriptor.shader.enable = true;
}

void SceneParser::parseGeneral(AttribMap& att)
{
    m_mapDescriptor.sceneName = att["name"];
    m_mapDescriptor.authorName = att["author"];
    m_mapDescriptor.ambiante.fromStr(att["ambient"]);

    if(att.count("znear"))
        m_mapDescriptor.znear = tools::strToNum<float>(att["znear"]);

    if(att.count("zfar"))
        m_mapDescriptor.zfar = tools::strToNum<float>(att["zfar"]);
}

void SceneParser::prepareNodeConstruction(Node* node, Relation& rel)
{
    if(tools::find(m_excludedNodes, node))
        return;

    rel.attr = node->constructionMap(m_filename);
    rel.deep = node->deepPosition() - 1;

    for(Iterator<Node*> it = node->getChildIterator(); it; it++)
    {
        Relation subrel;
        prepareNodeConstruction(*it, subrel);

        rel.child.push_back(subrel);
    }
}

void SceneParser::build()
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

    scene::Fog* fog = m_sceneManager->getFog();
    fog->setColor(m_mapDescriptor.fog.color);
    fog->setStart(m_mapDescriptor.fog.start);
    fog->setEnd(m_mapDescriptor.fog.end);
    fog->setEnable(m_mapDescriptor.fog.enable);

    Texture skytex[6];

    if(!m_mapDescriptor.skybox.front.empty())
        skytex[0] = m_mapDescriptor.skybox.front;

    if(!m_mapDescriptor.skybox.back.empty())
        skytex[1] = m_mapDescriptor.skybox.back;

    if(!m_mapDescriptor.skybox.top.empty())
        skytex[2] = m_mapDescriptor.skybox.top;

    if(!m_mapDescriptor.skybox.bottom.empty())
        skytex[3] = m_mapDescriptor.skybox.bottom;

    if(!m_mapDescriptor.skybox.left.empty())
        skytex[4] = m_mapDescriptor.skybox.left;

    if(!m_mapDescriptor.skybox.right.empty())
        skytex[5] = m_mapDescriptor.skybox.right;

    scene::SkyBox * sky = m_sceneManager->getSkybox();
    sky->setTextures(skytex);
    sky->setEnable(m_mapDescriptor.skybox.enable);

    if(m_mapDescriptor.shader.enable)
    {
        Shader shader;

        if(!m_mapDescriptor.shader.vert.empty())
            shader.loadVertexShader(tools::pathScope(m_filename, m_mapDescriptor.shader.vert, true));
        if(!m_mapDescriptor.shader.frag.empty())
            shader.loadFragmentShader(tools::pathScope(m_filename, m_mapDescriptor.shader.frag, true));

        shader.loadProgram();

        m_meshScene->setRenderingShader(shader);
    }

    m_sceneManager->setAmbientLight(m_mapDescriptor.ambiante);

    for(unsigned i = 0; i < m_mapDescriptor.nodes.size(); i++)
    {
        buildNode(m_mapDescriptor.nodes[i]);
    }

    m_sceneManager->setZNear(m_mapDescriptor.znear);
    m_sceneManager->setZFar(m_mapDescriptor.zfar);
    m_sceneManager->updateViewParameter();
}

SceneParser::MapDescriptor& SceneParser::getMapDescriptor()
{
    return m_mapDescriptor;
}

void SceneParser::setSceneName(std::string sceneName)
{
    m_mapDescriptor.sceneName = sceneName;
}

std::string SceneParser::getSceneName() const
{
    return m_mapDescriptor.sceneName;
}

void SceneParser::setAuthorName(std::string authorName)
{
    m_mapDescriptor.authorName = authorName;
}

std::string SceneParser::getAuthorName() const
{
    return m_mapDescriptor.authorName;
}

SceneParser::MapDescriptor::MapDescriptor()
{
    fog.start = 32;
    fog.end = 64;
    fog.enable = false;

    skybox.enable = false;
}
