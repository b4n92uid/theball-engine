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
#include "BurningEmitter.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

SceneParser::SceneParser(SceneManager* sceneManager)
: m_sceneManager(sceneManager), m_rootNode(sceneManager->GetRootNode()),
m_lightScene(NULL), m_meshScene(NULL), m_particlesScene(NULL), m_waterScene(NULL),
m_classFactory(NULL)
{
}

SceneParser::~SceneParser()
{
    delete m_classFactory;
}

void SceneParser::OutpuNodeConstruction(std::ofstream& file, Node* node)
{
    Node::CtorMap ctormap = node->ConstructionMap(m_fileName);

    string indent((node->DeepPosition() - 1) * 4, ' ');

    if(node->GetParent()->IsRoot())
        file << indent << "+node" << endl;

    for(Node::CtorMap::iterator it = ctormap.begin(); it != ctormap.end(); it++)
        file << indent << it->first << "=" << it->second << endl;

    for(Iterator<Node*> it = node->GetChildIterator(); it; it++)
    {
        string subindent((it->DeepPosition() - 1) * 4, ' ');

        file << subindent << "<" << endl;
        OutpuNodeConstruction(file, *it);
        file << subindent << ">" << endl;
    }

    if(node->GetParent()->IsRoot())
        file << endl;
}

void SceneParser::SaveScene()
{
    if(m_fileName.empty())
        return;

    SaveScene(m_fileName);
}

void SceneParser::SaveScene(const std::string& filepath)
{
    ofstream file(filepath.c_str());

    if(!file)
        throw tbe::Exception("SceneParser::SaveScene; Open file error (%s)", filepath.c_str());

    file << "*map" << endl;
    file << "name=" << m_mapName << endl;
    file << "ambient=" << m_sceneManager->GetAmbientLight() << endl;
    file << endl;

    Fog* fog = m_sceneManager->GetFog();

    if(fog)
    {
        file << "*fog" << endl;
        file << "color=" << fog->GetColor() << endl;
        file << "start=" << fog->GetStart() << endl;
        file << "end=" << fog->GetEnd() << endl;
        file << endl;
    }

    SkyBox* sky = m_sceneManager->GetSkybox();

    if(sky)
    {
        Texture* skyTexs = sky->GetTextures();

        file << "*skybox" << endl;
        file << "front=" << tools::makeRelatifTo(m_fileName, skyTexs[0].GetFilename()) << endl;
        file << "back=" << tools::makeRelatifTo(m_fileName, skyTexs[1].GetFilename()) << endl;
        file << "top=" << tools::makeRelatifTo(m_fileName, skyTexs[2].GetFilename()) << endl;
        file << "bottom=" << tools::makeRelatifTo(m_fileName, skyTexs[3].GetFilename()) << endl;
        file << "left=" << tools::makeRelatifTo(m_fileName, skyTexs[4].GetFilename()) << endl;
        file << "right=" << tools::makeRelatifTo(m_fileName, skyTexs[5].GetFilename()) << endl;
        file << endl;
    }

    Node* rootNode = m_sceneManager->GetRootNode();

    for(Iterator<Node*> it = rootNode->GetChildIterator(); it; it++)
        OutpuNodeConstruction(file, *it);

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

        if(buffer[rel.deep] == '>')
            return false;

        int deep = buffer.find("<");

        if(deep > rel.deep)
        {
            Relation subrel(deep);

            bool endofsec = parseBlock(file, subrel, line);

            rel.child.push_back(subrel);

            if(endofsec)
                return true;
        }

        else
        {
            string subbuffer = buffer.substr(rel.deep);

            unsigned sep = subbuffer.find_first_of('=');

            if(sep == string::npos)
                throw tbe::Exception("SceneParser::ParseRelation; invalid assignement %d (%s)", line, buffer.c_str());

            string key(subbuffer, 0, sep), value(subbuffer, sep + 1);

            rel.attr[key] = value;
        }
    }

    return true;
}

void SceneParser::LoadScene(const std::string& filepath)
{
    ifstream file(filepath.c_str());

    if(!file)
        throw tbe::Exception("BLDLoader::LoadScene; Open file error (%s)", filepath.c_str());

    m_fileName = filepath;

    string buffer;
    for(unsigned line = 0; tools::getline(file, buffer); line++)
    {
        if(buffer.empty() || buffer[0] == '#')
            continue;

        if(buffer == "*map")
        {
            Relation rel;
            parseBlock(file, rel, line);
            ParseMap(rel.attr);
        }

        else if(buffer == "*fog")
        {
            Relation rel;
            parseBlock(file, rel, line);
            ParseFog(rel.attr);
        }

        else if(buffer == "*skybox")
        {
            Relation rel;
            parseBlock(file, rel, line);
            ParseSkyBox(rel.attr);
        }

        else if(buffer == "+node")
        {
            Relation rel;
            parseBlock(file, rel, line);
            ParseNode(rel);
        }

        else if(buffer.substr(0, 8) == ".include")
        {
            string modelFilepath(buffer, 9);

            if(modelFilepath.find(':') == string::npos)
                modelFilepath = tools::makeRelatifTo(m_fileName, modelFilepath);

            LoadScene(modelFilepath);
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

void SceneParser::ParseMap(AttribMap& att)
{
    m_mapName = att["name"];
    m_sceneManager->SetAmbientLight(vec34(tools::StrToVec3<float>(att["ambient"], true)));

    m_lightScene = new LightParallelScene;
    m_sceneManager->AddParallelScene(m_lightScene);

    m_meshScene = new MeshParallelScene;
    m_sceneManager->AddParallelScene(m_meshScene);

    m_particlesScene = new ParticlesParallelScene;
    m_sceneManager->AddParallelScene(m_particlesScene);
}

void SceneParser::ParseFog(AttribMap& att)
{
    Vector4f color = tools::StrToVec4<float>(att["color"], true);
    float start = tools::StrToNum<float>(att["start"]);
    float end = tools::StrToNum<float>(att["end"]);

    scene::Fog* fog = m_sceneManager->GetFog();
    fog->SetColor(color);
    fog->SetStart(start);
    fog->SetEnd(end);
    fog->SetEnable(true);
}

void SceneParser::ParseSkyBox(AttribMap& att)
{
    Texture skyTex[6] = {
        tools::makeRelatifTo(m_fileName, att["front"]),
        tools::makeRelatifTo(m_fileName, att["back"]),
        tools::makeRelatifTo(m_fileName, att["top"]),
        tools::makeRelatifTo(m_fileName, att["bottom"]),
        tools::makeRelatifTo(m_fileName, att["left"]),
        tools::makeRelatifTo(m_fileName, att["right"])
    };

    scene::SkyBox* sky = m_sceneManager->GetSkybox();
    sky->SetTextures(skyTex);
    sky->SetEnable(true);
}

void SceneParser::ParseNode(Relation& rel, Node* parent)
{
    const string& nlass = rel.attr["class"];

    if(m_classRec.count(nlass))
    {
        Mesh* node = m_classFactory ? m_classFactory->Instance(nlass, m_meshScene) : new Mesh(m_meshScene);

        if(parent)
            parent->AddChild(node);
        else
            m_rootNode->AddChild(node);

        if(rel.attr.count("matrix"))
            node->SetMatrix(rel.attr["matrix"]);

        if(rel.attr.count("name"))
            node->SetName(rel.attr["name"]);

        for(unsigned i = 0; i < m_classRec[nlass].size(); i++)
            ParseNode(m_classRec[nlass][i], node);

        return;
    }

    Node* current = NULL;

    if(nlass == "OBJMesh")
    {
        OBJMesh* node = new OBJMesh(m_meshScene);

        string modelFilepath;

        if(rel.attr["filename"].find(':') != string::npos)
            modelFilepath = rel.attr["filename"];
        else
            modelFilepath = tools::makeRelatifTo(m_fileName, rel.attr["filename"]);

        node->Open(modelFilepath);

        current = node;
    }

    else if(nlass == "ParticlesEmiter")
    {
        BurningEmitter* emiter = new BurningEmitter(m_particlesScene);

        string modelFilepath;

        if(rel.attr["texture"].find(':') != string::npos)
            modelFilepath = rel.attr["texture"];
        else
            modelFilepath = tools::makeRelatifTo(m_fileName, rel.attr["texture"]);

        emiter->SetTexture(modelFilepath);

        emiter->SetLifeInit(tools::StrToNum<float>(rel.attr["lifeInit"]));
        emiter->SetLifeDown(tools::StrToNum<float>(rel.attr["lifeDown"]));

        emiter->SetGravity(tools::StrToVec3<float>(rel.attr["gravity"], true));

        emiter->SetNumber(tools::StrToNum<int>(rel.attr["number"]));

        emiter->SetFreeMove(tools::StrToNum<float>(rel.attr["freeMove"]));

        emiter->SetContinousMode(tools::StrToNum<bool>(rel.attr["continousMode"]));

        emiter->Build();

        current = emiter;
    }

    else if(nlass == "Light")
    {
        Light* light = NULL;

        if(rel.attr["type"] == "Diri")
        {
            light = new DiriLight(m_lightScene);
        }

        else if(rel.attr["type"] == "Point")
        {
            float radius = tools::StrToNum<float>(rel.attr["radius"]);

            light = new PointLight(m_lightScene);
            light->SetRadius(radius);
        }

        else
            throw tbe::Exception("SceneParser::ParseNode; Unknown light type (%s)", rel.attr["type"].c_str());

        light->SetAmbient(tools::StrToVec4<float>(rel.attr["ambient"], true));
        light->SetDiffuse(tools::StrToVec4<float>(rel.attr["diffuse"], true));
        light->SetSpecular(tools::StrToVec4<float>(rel.attr["specular"], true));

        current = light;
    }

    else
    {
        BullNode* node = new BullNode;

        current = node;
    }

    if(parent)
        parent->AddChild(current);
    else
        m_rootNode->AddChild(current);

    if(rel.attr.count("name"))
        current->SetName(rel.attr["name"]);

    if(rel.attr.count("matrix"))
        current->SetMatrix(rel.attr["matrix"]);

    for(unsigned i = 0; i < rel.child.size(); i++)
        ParseNode(rel.child[i], current);
}

void SceneParser::SetClassFactory(ClassFactory* classFactory)
{
    this->m_classFactory = classFactory;
}

ClassFactory* SceneParser::GetClassFactory() const
{
    return m_classFactory;
}

ParticlesParallelScene* SceneParser::GetParticlesScene() const
{
    return m_particlesScene;
}

MeshParallelScene* SceneParser::GetMeshScene() const
{
    return m_meshScene;
}

WaterParallelScene* SceneParser::GetWaterScene() const
{
    return m_waterScene;
}

LightParallelScene* SceneParser::GetLightScene() const
{
    return m_lightScene;
}
