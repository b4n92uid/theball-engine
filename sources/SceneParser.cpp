/*
 * File:   SceneParser.cpp
 * Author: b4n92uid
 *
 * Created on 5 novembre 2010, 14:19
 */

#include "SceneParser.h"
#include "ObjMesh.h"

#include <fstream>

#include "SceneManager.h"
#include "Skybox.h"
#include "Tools.h"
#include "Particles.h"
#include "Water.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

SceneParser::SceneParser(SceneManager* sceneManager) :
m_sceneManager(sceneManager),
m_rootNode(sceneManager->getRootNode()),
m_lightScene(NULL),
m_meshScene(NULL),
m_particlesScene(NULL),
m_waterScene(NULL)
{
}

SceneParser::~SceneParser()
{
}

void SceneParser::outpuNodeConstruction(std::ofstream& file, Node* node)
{
    if(tools::find(m_excludedNodes, node))
        return;

    Node::CtorMap ctormap = node->constructionMap(m_fileName);

    if(node->isRoot())
    {
        for(Iterator<Node*> it = node->getChildIterator(); it; it++)
            outpuNodeConstruction(file, *it);

        return;
    }

    string indent((node->deepPosition() - 1) * 4, ' ');

    if(node->getParent()->isRoot())
        file << indent << "+node" << endl;

    for(Node::CtorMap::iterator it = ctormap.begin(); it != ctormap.end(); it++)
        file << indent << it->first << "=" << it->second << endl;

    for(Iterator<Node*> it = node->getChildIterator(); it; it++)
    {
        file << indent << "{" << endl;
        outpuNodeConstruction(file, *it);
        file << indent << "}" << endl;
    }

    if(node->getParent()->isRoot())
        file << endl;
}

SceneParser& SceneParser::exclude(Node *node)
{
    m_excludedNodes.push_back(node);
    return *this;
}

SceneParser& SceneParser::archive(Node *node)
{
    bool skip = false, done = false;

    while(!done)
    {
        done = true;

        for(unsigned i = 0; i < m_archivedNodes.size(); i++)
            if(m_archivedNodes[i] == node)
            {
                skip = true;
                break;
            }
            else if(m_archivedNodes[i]->isChild(node, true))
            {
                skip = true;
                break;
            }
            else if(m_archivedNodes[i]->isParent(node))
            {
                skip = true;
                done = false;
                m_archivedNodes[i] = node;
                break;
            }
    }

    if(!skip)
        m_archivedNodes.push_back(node);

    return *this;
}

void SceneParser::saveClass()
{

}

void SceneParser::saveClass(const std::string& filepath)
{

}

void SceneParser::loadClass(const std::string& filepath)
{

}

void SceneParser::saveScene()
{
    if(m_fileName.empty())
        return;

    saveScene(m_fileName);
}

void SceneParser::saveScene(const std::string& filepath)
{
    ofstream file(filepath.c_str());

    if(!file)
        throw tbe::Exception("SceneParser::SaveScene; Open file error (%s)", filepath.c_str());

    m_fileName = filepath;

    file << "*general" << endl;
    file << "name=" << m_sceneName << endl;
    file << "author=" << m_authorName << endl;
    file << "ambient=" << m_sceneManager->getAmbientLight() << endl;
    file << endl;

    if(m_additional.size())
    {
        file << "*additional" << endl;
        for(AttribMap::iterator it = m_additional.begin(); it != m_additional.end(); it++)
            file << it->first << "=" << it->second << endl;
        file << endl;
    }

    Fog* fog = m_sceneManager->getFog();

    if(fog->isEnable())
    {
        file << "*fog" << endl;
        file << "color=" << fog->getColor() << endl;
        file << "start=" << fog->getStart() << endl;
        file << "end=" << fog->getEnd() << endl;
        file << endl;
    }

    SkyBox* sky = m_sceneManager->getSkybox();

    if(sky->isEnable())
    {
        Texture* skyTexs = sky->getTextures();

        file << "*skybox" << endl;
        file << "front=" << tools::pathScope(m_fileName, skyTexs[0].getFilename(), false) << endl;
        file << "back=" << tools::pathScope(m_fileName, skyTexs[1].getFilename(), false) << endl;
        file << "top=" << tools::pathScope(m_fileName, skyTexs[2].getFilename(), false) << endl;
        file << "bottom=" << tools::pathScope(m_fileName, skyTexs[3].getFilename(), false) << endl;
        file << "left=" << tools::pathScope(m_fileName, skyTexs[4].getFilename(), false) << endl;
        file << "right=" << tools::pathScope(m_fileName, skyTexs[5].getFilename(), false) << endl;
        file << endl;
    }

    for(unsigned it = 0; it < m_archivedNodes.size(); it++)
        outpuNodeConstruction(file, m_archivedNodes[it]);

    m_archivedNodes.clear();

    file.close();
}

bool SceneParser::parseBlock(std::ifstream& file, Relation& rel, unsigned& line)
{
    string buffer;
    while(tools::getline(file, buffer))
    {
        line++;

        if(buffer.empty())
            return true;

        if(buffer[0] == '#')
            continue;

        if(buffer.find("}") != string::npos)
            return false;

        int deep = rel.deep;

        if(buffer.find("{") != string::npos)
        {
            deep++;

            Relation subrel(deep);

            bool endofsec = parseBlock(file, subrel, line);

            rel.child.push_back(subrel);

            if(endofsec)
                return true;
        }

        else
        {
            tools::trimstr(buffer);

            unsigned sep = buffer.find_first_of('=');

            if(sep == string::npos)
                throw tbe::Exception("SceneParser::parseRelation; invalid assignement %d (%s)", line, buffer.c_str());

            string key(buffer, 0, sep), value(buffer, sep + 1);

            rel.attr[key] = value;
        }
    }

    return true;
}

void SceneParser::loadScene(const std::string& filepath)
{
    ifstream file(filepath.c_str());

    if(!file)
        throw tbe::Exception("SceneParser::loadScene; Open file error (%s)", filepath.c_str());

    m_fileName = filepath;

    string buffer;
    for(unsigned line = 0; tools::getline(file, buffer); line++)
    {
        if(buffer.empty() || buffer[0] == '#')
            continue;

        if(buffer == "*general")
        {
            Relation rel;
            parseBlock(file, rel, line);
            parseGeneral(rel.attr);
        }

        else if(buffer == "*additional")
        {
            Relation rel;
            parseBlock(file, rel, line);
            m_additional = rel.attr;
        }

        else if(buffer == "*fog")
        {
            Relation rel;
            parseBlock(file, rel, line);
            parseFog(rel.attr);
        }

        else if(buffer == "*skybox")
        {
            Relation rel;
            parseBlock(file, rel, line);
            parseSkyBox(rel.attr);
        }

        else if(buffer == "+node")
        {
            Relation rel;
            parseBlock(file, rel, line);
            parseNode(rel);
        }

        else if(buffer.substr(0, 8) == ".include")
        {
            string modelFilepath(buffer, 9);

            if(modelFilepath.find(':') == string::npos)
                modelFilepath = tools::pathScope(m_fileName, modelFilepath, true);

            loadScene(modelFilepath);
        }

        else if(buffer.substr(0, 6) == "/class")
        {
            Relation rel;
            parseBlock(file, rel, line);

            m_classRec[buffer.substr(7)] = rel.child;
        }

        else
            throw tbe::Exception("SceneParser::LoadScene; Parse error %d (%s)", line, buffer.c_str());
    }

    file.close();
}

void SceneParser::parseGeneral(AttribMap& att)
{
    m_sceneName = att["name"];
    m_authorName = att["author"];

    m_sceneManager->setAmbientLight(vec34(tools::strToVec3<float>(att["ambient"], true)));

    if(!m_lightScene)
    {
        m_lightScene = new LightParallelScene;
        m_sceneManager->addParallelScene(m_lightScene);
    }

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
}

void SceneParser::parseFog(AttribMap& att)
{
    Vector4f color = tools::strToVec4<float>(att["color"], true);
    float start = tools::strToNum<float>(att["start"]);
    float end = tools::strToNum<float>(att["end"]);

    scene::Fog* fog = m_sceneManager->getFog();
    fog->setColor(color);
    fog->setStart(start);
    fog->setEnd(end);
    fog->setEnable(true);
}

void SceneParser::parseSkyBox(AttribMap& att)
{
    Texture skyTex[6];

    if(!att["front"].empty())
        skyTex[0] = tools::pathScope(m_fileName, att["front"], true);

    if(!att["back"].empty())
        skyTex[1] = tools::pathScope(m_fileName, att["back"], true);

    if(!att["top"].empty())
        skyTex[2] = tools::pathScope(m_fileName, att["top"], true);

    if(!att["bottom"].empty())
        skyTex[3] = tools::pathScope(m_fileName, att["bottom"], true);

    if(!att["left"].empty())
        skyTex[4] = tools::pathScope(m_fileName, att["left"], true);

    if(!att["right"].empty())
        skyTex[5] = tools::pathScope(m_fileName, att["right"], true);

    scene::SkyBox * sky = m_sceneManager->getSkybox();
    sky->setTextures(skyTex);
    sky->setEnable(true);
}

void SceneParser::parseNode(Relation& rel, Node* parent)
{
    const string& iclass = rel.attr["class"];

    if(m_classRec.count(iclass))
    {
        Node* node = new BullNode;

        if(parent)
            parent->addChild(node);
        else
            m_rootNode->addChild(node);

        if(rel.attr.count("matrix"))
            node->setMatrix(rel.attr["matrix"]);

        if(rel.attr.count("name"))
            node->setName(rel.attr["name"]);

        for(unsigned i = 0; i < m_classRec[iclass].size(); i++)
            parseNode(m_classRec[iclass][i], node);

        return;
    }

    Node* current = NULL;

    if(iclass == "OBJMesh")
    {
        OBJMesh* node = new OBJMesh(m_meshScene);

        string modelFilepath;

        if(rel.attr["filename"].find(':') != string::npos)
            modelFilepath = rel.attr["filename"];
        else
            modelFilepath = tools::pathScope(m_fileName, rel.attr["filename"], true);

        node->open(modelFilepath);

        current = node;
    }

    else if(iclass == "ParticlesEmiter")
    {
        ParticlesEmiter* emiter = new ParticlesEmiter(m_particlesScene);

        string modelFilepath;

        if(rel.attr["texture"].find(':') != string::npos)
            modelFilepath = rel.attr["texture"];
        else
            modelFilepath = tools::pathScope(m_fileName, rel.attr["texture"], true);

        emiter->setTexture(modelFilepath);

        emiter->setLifeInit(tools::strToNum<float>(rel.attr["lifeInit"]));
        emiter->setLifeDown(tools::strToNum<float>(rel.attr["lifeDown"]));

        emiter->setGravity(tools::strToVec3<float>(rel.attr["gravity"], true));
        emiter->setBoxSize(tools::strToVec3<float>(rel.attr["boxSize"], true));

        emiter->setNumber(tools::strToNum<int>(rel.attr["number"]));

        emiter->setFreeMove(tools::strToNum<float>(rel.attr["freeMove"]));

        emiter->setContinousMode(tools::strToNum<bool>(rel.attr["continousMode"]));

        emiter->build();

        current = emiter;
    }

    else if(iclass == "Light")
    {
        Light* light = new Light(m_lightScene);

        if(rel.attr["type"] == "Diri")
        {
            light->setType(Light::DIRI);
        }

        else if(rel.attr["type"] == "Point")
        {
            float radius = tools::strToNum<float>(rel.attr["radius"]);

            light->setType(Light::POINT);
            light->setRadius(radius);
        }

        else
            throw tbe::Exception("SceneParser::ParseNode; Unknown light type (%s)", rel.attr["type"].c_str());

        light->setAmbient(tools::strToVec4<float>(rel.attr["ambient"], true));
        light->setDiffuse(tools::strToVec4<float>(rel.attr["diffuse"], true));
        light->setSpecular(tools::strToVec4<float>(rel.attr["specular"], true));

        current = light;
    }

    else if(iclass == "BullNode")
    {
        current = new BullNode;
    }

    else
        throw Exception("SceneParser::parseNode; Unknown class (%s)", iclass.c_str());

    if(current)
    {
        if(parent)
            parent->addChild(current);
        else
            m_rootNode->addChild(current);

        if(rel.attr.count("name"))
            current->setName(rel.attr["name"]);

        if(rel.attr.count("matrix"))
            current->setMatrix(rel.attr["matrix"]);

        for(AttribMap::iterator it = rel.attr.begin(); it != rel.attr.end(); it++)
        {
            if(it->first[0] == '.')
            {
                string key(it->first, 1);
                current->setUserData(key, it->second);
            }
        }

        for(unsigned i = 0; i < rel.child.size(); i++)
            parseNode(rel.child[i], current);
    }
}

const SceneParser::AttribMap SceneParser::additionalFields() const
{
    return m_additional;
}

void SceneParser::clearAdditional()
{
    m_additional.clear();
}

void SceneParser::removeAdditional(std::string key)
{
    if(m_additional.count(key))
        m_additional.erase(key);
}

void SceneParser::setSceneName(std::string sceneName)
{
    this->m_sceneName = sceneName;
}

std::string SceneParser::getSceneName() const
{
    return m_sceneName;
}

void SceneParser::setAuthorName(std::string authorName)
{
    this->m_authorName = authorName;
}

std::string SceneParser::getAuthorName() const
{
    return m_authorName;
}

ParticlesParallelScene* SceneParser::getParticlesScene() const
{
    return m_particlesScene;
}

MeshParallelScene* SceneParser::getMeshScene() const
{
    return m_meshScene;
}

WaterParallelScene* SceneParser::getWaterScene() const
{
    return m_waterScene;
}

LightParallelScene* SceneParser::getLightScene() const
{
    return m_lightScene;
}

void SceneParser::setWaterScene(WaterParallelScene* waterScene)
{
    this->m_waterScene = waterScene;
}

void SceneParser::setParticlesScene(ParticlesParallelScene* particlesScene)
{
    this->m_particlesScene = particlesScene;
}

void SceneParser::setMeshScene(MeshParallelScene* meshScene)
{
    this->m_meshScene = meshScene;
}

void SceneParser::setLightScene(LightParallelScene* lightScene)
{
    this->m_lightScene = lightScene;
}
