/* 
 * File:   BldParser.cpp
 * Author: b4n92uid
 * 
 * Created on 5 novembre 2010, 14:19
 */

#include "BldParser.h"

#include <fstream>

#include "SceneManager.h"
#include "Skybox.h"
#include "Tools.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

BldParser::BldParser(SceneManager* sceneManager) : m_sceneManager(sceneManager)
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

void BldParser::ParseMap(AttribMap& att)
{
    m_mapName = att["name"];
    m_sceneManager->SetAmbientLight(vec34(Vector3f(att["ambient"])));
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

void BldParser::ParseClass(AttribMap& att)
{

}

void BldParser::ParseNode(AttribMap& att)
{
    throw tbe::Exception("BLDLoader::ParseNode; Unknown node type (%s)", att["type"].c_str());
}

void BldParser::ParseLight(AttribMap& att)
{
    Light* light = NULL;

    if(att["type"] == "DIRI")
    {
        light = new DiriLight;
    }

    else if(att["type"] == "POINT")
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
