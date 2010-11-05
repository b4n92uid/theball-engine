/* 
 * File:   BldParser.cpp
 * Author: b4n92uid
 * 
 * Created on 5 novembre 2010, 14:19
 */

#include "BldParser.h"
#include "ObjMesh.h"

#include <fstream>

#include "SceneManager.h"
#include "Skybox.h"
#include "Tools.h"
#include "Particles.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

BldParser::BldParser(SceneManager* sceneManager)
: m_sceneManager(sceneManager), m_meshScene(NULL)
{
}

BldParser::~BldParser()
{
}

BldParser::AttribMap BldParser::GetAttributs(std::ifstream& file)
{
    BldParser::AttribMap fileMap;
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

void BldParser::LoadScene(const std::string& filepath)
{
    ifstream file(filepath.c_str());

    if(!file)
        throw tbe::Exception("BLDLoader::LoadScene; Open file error (%s)", filepath.c_str());

    string buffer;
    for(unsigned line = 0; tools::getline(file, buffer); line++)
    {
        if(buffer.empty() || buffer[0] == '#')
            continue;

        if(buffer == ".map")
        {
            AttribMap att = GetAttributs(file);
            ParseMap(att);
        }

        else if(buffer == ".music")
        {
        }

        else if(buffer == ".fog")
        {
            AttribMap att = GetAttributs(file);
            ParseFog(att);
        }

        else if(buffer == ".skybox")
        {
            AttribMap att = GetAttributs(file);
            ParseSkyBox(att);
        }

        else if(buffer == "+node")
        {
            AttribMap att = GetAttributs(file);
            ParseNode(att);
        }

        else if(buffer == "+light")
        {
            AttribMap att = GetAttributs(file);
            ParseLight(att);
        }

        else if(buffer.substr(0, 6) == "/class")
        {
            RecordClass(file, buffer.substr(7));
        }

        else
            throw tbe::Exception("BldParser::LoadScene; Parse error %d (%s)", line, buffer.c_str());
    }

    file.close();
}

void BldParser::ParseMap(AttribMap& att)
{
    m_mapName = att["name"];
    m_sceneManager->SetAmbientLight(vec34(Vector3f(att["ambient"])));

    m_meshScene = new MeshParallelScene;
    m_sceneManager->AddParallelScene(m_mapName + ":Mesh", m_meshScene);

    m_particleScene = new ParticlesParallelScene;
    m_sceneManager->AddParallelScene(m_mapName + ":Particles", m_particleScene);
}

void BldParser::ParseFog(AttribMap& att)
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

void BldParser::ParseSkyBox(AttribMap& att)
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

void BldParser::RecordClass(std::ifstream& file, std::string name)
{
    Mesh* mesh = m_classRec[name] = new Mesh;

    string buffer;

    while(tools::getline(file, buffer))
    {
        if(buffer.empty())
            break;

        tools::trimstr(buffer);

        if(buffer == "+node")
        {
            AttribMap attribs = GetAttributs(file);
            ParseNode(attribs, mesh);
        }
    }
}

void BldParser::ParseNode(AttribMap& att, Mesh* parent)
{
    if(att["type"] == "OBJMesh")
    {
        OBJMesh* mesh = new OBJMesh;
        mesh->Open(att["open"]);

        if(parent)
            ;
        else
            m_meshScene->AddMesh("", mesh);
    }

    else if(att["type"] == "ParticlesEmiter")
    {
        //        ParticlesEmiter* emiter = new ParticlesEmiter;

        //        m_particleScene->AddParticlesEmiter("", emiter);
    }

    else if(m_classRec.count(att["type"]))
    {
        //        Mesh* mesh = m_classRec[att["type"]];

        //        m_meshScene->AddMesh("", mesh);
    }

    else
        throw tbe::Exception("BLDLoader::ParseNode; Unknown node type (%s)", att["type"].c_str());
}

void BldParser::ParseLight(AttribMap& att)
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
