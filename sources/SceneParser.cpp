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

SceneParser::AttribMap SceneParser::GetAttributs(std::ifstream& file)
{
    SceneParser::AttribMap fileMap;
    string buffer;

    while(tools::getline(file, buffer))
    {
        tools::trimstr(buffer);

        if(buffer.empty())
            break;

        if(buffer[0] == '#')
            continue;

        string key(buffer, 0, buffer.find_first_of('=')),
                value(buffer, buffer.find_first_of('=') + 1);

        fileMap[key] = value;
    }

    return fileMap;
}

void SceneParser::LoadScene(const std::string& filepath)
{
    ifstream file(filepath.c_str());

    if(!file)
        throw tbe::Exception("BLDLoader::LoadScene; Open file error (%s)", filepath.c_str());

    string buffer;
    for(unsigned line = 0; tools::getline(file, buffer); line++)
    {
        if(buffer.empty() || buffer[0] == '#')
            continue;

        if(buffer == "*map")
        {
            AttribMap att = GetAttributs(file);
            ParseMap(att);
        }

        else if(buffer == "*fog")
        {
            AttribMap att = GetAttributs(file);
            ParseFog(att);
        }

        else if(buffer == "*skybox")
        {
            AttribMap att = GetAttributs(file);
            ParseSkyBox(att);
        }

        else if(buffer == "+light")
        {
            AttribMap att = GetAttributs(file);
            ParseLight(att);
        }

        else if(buffer == "+node")
        {
            AttribMap att = GetAttributs(file);
            ParseNode(att);
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

void SceneParser::RecordClass(std::ifstream& file, std::string type)
{
    string buffer;

    while(tools::getline(file, buffer))
    {
        if(buffer.empty())
            break;

        tools::trimstr(buffer);

        if(buffer[0] == '#')
            continue;

        if(buffer == "+node")
            m_classRec[type].push_back(GetAttributs(file));
    }
}

void SceneParser::ParseNode(AttribMap& att, Mesh* parent)
{
    const string& type = att["type"];

    if(type == "OBJMesh")
    {
        OBJMesh* mesh = new OBJMesh;
        mesh->Open(att["open"]);
        mesh->SetMatrix(att["matrix"]);

        if(parent)
            parent->AddChild(mesh);
        else
            m_meshScene->AddMesh("", mesh);
    }

    else if(type == "ParticlesEmiter")
    {
        ParticlesEmiter* emiter = new ParticlesEmiter;

        emiter->SetTexture(att["open"]);

        emiter->SetMatrix(att["matrix"]);
        emiter->SetEndPos(att["endPos"]);

        emiter->SetLifeInit(tools::StrToNum<float>(att["lifeInit"]));
        emiter->SetLifeDown(tools::StrToNum<float>(att["lifeDown"]));

        emiter->SetGravity(att["gravity"]);

        emiter->SetNumber(tools::StrToNum<int>(att["number"]));

        emiter->SetFreeMove(tools::StrToNum<float>(att["freemove"]));

        emiter->SetContinousMode(tools::StrToNum<bool>(att["continous"]));

        emiter->Build();

        emiter->SetMatrixParent(parent);

        m_particleScene->AddParticlesEmiter("", emiter);
    }

    else if(m_classRec.count(type))
    {
        Mesh* mesh = m_classFactory ? m_classFactory->Instance(type) : new Mesh;

        mesh->SetMatrix(att["matrix"]);

        AttribMapArray& attArray = m_classRec[type];

        for(unsigned i = 0; i < attArray.size(); i++)
            ParseNode(attArray[i], mesh);

        if(parent)
            parent->AddChild(mesh);
        else
            m_meshScene->AddMesh("", mesh);
    }

    else
        throw tbe::Exception("BLDLoader::ParseNode; Unknown node type (%s)", type.c_str());
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

void SceneParser::SetClassFactory(ClassFactory* classFactory)
{
    this->m_classFactory = classFactory;
}

ClassFactory* SceneParser::GetClassFactory() const
{
    return m_classFactory;
}
