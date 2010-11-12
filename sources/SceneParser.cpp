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

using namespace std;
using namespace tbe;
using namespace tbe::scene;

SceneParser::SceneParser(SceneManager* sceneManager)
: m_sceneManager(sceneManager), m_meshScene(NULL), m_classFactory(NULL)
{
}

SceneParser::~SceneParser()
{
    delete m_classFactory;
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
        throw tbe::Exception("BLDLoader::SaveScene; Open file error (%s)", filepath.c_str());

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
        file << "front=" << skyTexs[0].GetFilename() << endl;
        file << "back=" << skyTexs[1].GetFilename() << endl;
        file << "top=" << skyTexs[2].GetFilename() << endl;
        file << "bottom=" << skyTexs[3].GetFilename() << endl;
        file << "left=" << skyTexs[4].GetFilename() << endl;
        file << "right=" << skyTexs[5].GetFilename() << endl;
        file << endl;
    }

    file.close();
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
            ParseRelation(file, rel);
            ParseMap(rel.attr);
        }

        else if(buffer == "*fog")
        {
            Relation rel;
            ParseRelation(file, rel);
            ParseFog(rel.attr);
        }

        else if(buffer == "*skybox")
        {
            Relation rel;
            ParseRelation(file, rel);
            ParseSkyBox(rel.attr);
        }

        else if(buffer == "+light")
        {
            Relation rel;
            ParseRelation(file, rel);
            ParseLight(rel.attr);
        }

        else if(buffer == "+node")
        {
            Relation rel;
            ParseRelation(file, rel);
            ParseNode(rel);
        }

        else if(buffer.substr(0, 6) == "/class")
        {
            RecordClass(file, buffer.substr(7));
        }

        else
            throw tbe::Exception("SceneParser::LoadScene; Parse error %d (%s)", line, buffer.c_str());
    }

    file.close();
}

void SceneParser::ParseMap(AttribMap& att)
{
    m_mapName = att["name"];
    m_sceneManager->SetAmbientLight(vec34(Vector3f(att["ambient"])));

    m_meshScene = new MeshParallelScene;
    m_sceneManager->AddParallelScene(m_mapName + ":Mesh", m_meshScene);

    m_particleScene = new ParticlesParallelScene;
    m_sceneManager->AddParallelScene(m_mapName + ":Particles", m_particleScene);
}

void SceneParser::ParseFog(AttribMap& att)
{
    Vector4f color = att["color"];
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
        att["front"],
        att["back"],
        att["top"],
        att["bottom"],
        att["left"],
        att["right"]
    };

    scene::SkyBox* sky = m_sceneManager->GetSkybox();
    sky->SetTextures(skyTex);
    sky->SetEnable(true);
}

void SceneParser::ParseLight(AttribMap& att)
{
    Light* light = NULL;

    if(att["type"] == "Diri")
    {
        light = new DiriLight;
    }

    else if(att["type"] == "Point")
    {
        float radius = tools::StrToNum<float>(att["radius"]);

        light = new PointLight;
        light->SetRadius(radius);
    }

    else
        throw tbe::Exception("BLDLoader::LoadScene; Unknown light type (%s)", att["type"].c_str());

    light->SetPos(att["pos"]);

    light->SetAmbient(att["ambient"]);
    light->SetDiffuse(att["diffuse"]);
    light->SetSpecular(att["specular"]);

    m_sceneManager->AddDynamicLight(tools::numToStr(light), light);
}

void SceneParser::ParseRelation(std::ifstream& file, Relation& rel)
{
    string buffer;

    while(tools::getline(file, buffer))
    {
        if(buffer.empty())
            break;

        buffer = buffer.substr(rel.deep);

        if(buffer.empty())
            break;

        if(buffer[0] == '#' || tools::isspace(buffer))
            continue;

        int deep = buffer.find("+node");

        if(deep > rel.deep)
        {
            Relation subrel(deep);
            ParseRelation(file, subrel);

            rel.child.push_back(subrel);

            continue;
        }

        string key(buffer, 0, buffer.find_first_of('=')),
                value(buffer, buffer.find_first_of('=') + 1);

        rel.attr[key] = value;
    }
}

void SceneParser::RecordClass(std::ifstream& file, std::string type)
{
    Relation rel;
    ParseRelation(file, rel);

    m_classRec[type] = rel.child;
}

void SceneParser::ParseNode(Relation& rel, Node* parent)
{
    const string& type = rel.attr["type"];

    if(m_classRec.count(type))
    {
        Node* node = m_classFactory ? m_classFactory->Instance(type) : new Mesh;

        if(rel.attr.count("matrix"))
            node->SetMatrix(rel.attr["matrix"]);

        if(rel.attr.count("name"))
            node->SetName(rel.attr["name"]);

        if(parent)
            node->SetParent(parent);

        m_meshScene->AddChild(node);

        for(unsigned i = 0; i < m_classRec[type].size(); i++)
            ParseNode(m_classRec[type][i], node);

        return;
    }

    Node* current = NULL;

    if(type == "OBJMesh")
    {
        OBJMesh* node = new OBJMesh;
        node->Open(rel.attr["open"]);

        if(rel.attr.count("matrix"))
            node->SetMatrix(rel.attr["matrix"]);

        if(rel.attr.count("name"))
            node->SetName(rel.attr["name"]);

        if(parent)
            node->SetParent(parent);

        m_meshScene->AddChild(node);

        current = node;
    }

    else if(type == "ParticlesEmiter")
    {
        ParticlesEmiter* emiter = new ParticlesEmiter;

        emiter->SetTexture(rel.attr["open"]);

        emiter->SetMatrix(rel.attr["matrix"]);
        emiter->SetEndPos(rel.attr["endPos"]);

        emiter->SetLifeInit(tools::StrToNum<float>(rel.attr["lifeInit"]));
        emiter->SetLifeDown(tools::StrToNum<float>(rel.attr["lifeDown"]));

        emiter->SetGravity(rel.attr["gravity"]);

        emiter->SetNumber(tools::StrToNum<int>(rel.attr["number"]));

        emiter->SetFreeMove(tools::StrToNum<float>(rel.attr["freemove"]));

        emiter->SetContinousMode(tools::StrToNum<bool>(rel.attr["continous"]));

        emiter->Build();

        if(parent)
            emiter->SetParent(parent);

        m_particleScene->AddChild(emiter);

        current = emiter;
    }

    else
        throw tbe::Exception("BLDLoader::ParseNode; Unknown node type (%s)", type.c_str());

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
