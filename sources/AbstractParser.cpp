/*
 * File:   AbstractParser.cpp
 * Author: b4n92uid
 *
 * Created on 21 mars 2012, 13:11
 */

#include "AbstractParser.h"

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

AbstractParser::AbstractParser()
{
    m_sceneManager = NULL;
    m_rootNode = NULL;
    m_classFactory = NULL;
    m_lightScene = NULL;
    m_meshScene = NULL;
    m_particlesScene = NULL;
    m_markScene = NULL;
    m_version = 0.1;
    m_parseLine = 0;
}

AbstractParser::AbstractParser(SceneManager* sceneManager)
{
    m_sceneManager = sceneManager;
    m_rootNode = sceneManager->getRootNode();
    m_classFactory = NULL;
    m_lightScene = NULL;
    m_meshScene = NULL;
    m_particlesScene = NULL;
    m_markScene = NULL;
    m_version = 0.1;
    m_parseLine = 0;
}

AbstractParser::~AbstractParser()
{
    delete m_classFactory;
}

std::string AbstractParser::getFilename() const
{
    return m_filename;
}

void AbstractParser::clear()
{
    m_version = 0.0;
    m_parseLine = 0;
    m_additional.clear();
    m_filename.clear();
}

ParticlesParallelScene* AbstractParser::getParticlesScene() const
{
    return m_particlesScene;
}

MeshParallelScene* AbstractParser::getMeshScene() const
{
    return m_meshScene;
}

LightParallelScene* AbstractParser::getLightScene() const
{
    return m_lightScene;
}

MapMarkParallelScene* AbstractParser::getMarkScene() const
{
    return m_markScene;
}

void AbstractParser::setClassFactory(ParserClassFactory* classFactory)
{
    this->m_classFactory = classFactory;
}

ParserClassFactory* AbstractParser::getClassFactory() const
{
    return m_classFactory;
}

void AbstractParser::setParticlesScene(ParticlesParallelScene* particlesScene)
{
    this->m_particlesScene = particlesScene;
}

void AbstractParser::setMeshScene(MeshParallelScene* meshScene)
{
    this->m_meshScene = meshScene;
}

void AbstractParser::setLightScene(LightParallelScene* lightScene)
{
    this->m_lightScene = lightScene;
}

void AbstractParser::setMarkScene(MapMarkParallelScene* markScene)
{
    this->m_markScene = markScene;
}

bool AbstractParser::parseBlock(std::ifstream& file, AbstractParser::Relation& rel)
{
    string buffer;
    while(tools::getline(file, buffer))
    {
        m_parseLine++;

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

            bool endofsec = parseBlock(file, subrel);

            rel.child.push_back(subrel);

            if(endofsec)
                return true;
        }

        else
        {
            buffer = tools::trim(buffer);

            unsigned sep = buffer.find_first_of('=');

            if(sep == string::npos)
                throw tbe::Exception("SceneParser::parseRelation; invalid assignement (%s:%d)", m_filename.c_str(), m_parseLine);

            string key(buffer, 0, sep), value(buffer, sep + 1);

            rel.attr[key] = value;
        }
    }

    return true;
}

void AbstractParser::outpuNodeConstruction(Relation& rel, std::ofstream& file)
{
    string indent((rel.deep) * 4, ' ');

    if(rel.deep == 0)
        file << indent << "+node" << endl;

    for(AttribMap::iterator it = rel.attr.begin(); it != rel.attr.end(); it++)
        file << indent << it->first << "=" << it->second << endl;

    for(unsigned i = 0; i < rel.child.size(); i++)
    {
        file << indent << "{" << endl;
        outpuNodeConstruction(rel.child[i], file);
        file << indent << "}" << endl;
    }

    if(rel.deep == 0)
        file << endl;
}

const AbstractParser::AttribMap AbstractParser::additionalFields() const
{
    return m_additional;
}

void AbstractParser::clearAdditional()
{
    m_additional.clear();
}

void AbstractParser::removeAdditional(std::string key)
{
    if(m_additional.count(key))
        m_additional.erase(key);
}

std::string AbstractParser::getAdditionalString(std::string key)
{
    if(m_additional.count(key))
        return m_additional[key];
    else
        return std::string();
}

void AbstractParser::setAdditionalString(std::string key, std::string value)
{
    m_additional[key] = value;
}

inline void toogleMaterial(Material* material, int mod, string stat)
{
    tools::strToNum<bool>(stat)
            ? material->enable(mod)
            : material->disable(mod);
}

void AbstractParser::buildMaterial(AttribMap attr, Mesh* mesh)
{
    for(AttribMap::iterator it = attr.begin(); it != attr.end(); it++)
    {
        if(it->first[0] != '!')
            continue;

        mesh->setOutputMaterial(true);

        string key = it->first;
        key.erase(0, 1);

        buildMaterial(key, it->second, mesh);
    }
}

void AbstractParser::buildMaterial(std::string key, std::string value, Mesh* mesh)
{
    vector<string> token = tools::tokenize(key, ':');

    if(token.empty() || token.size() < 2)
        return;

    string matname = token[0];
    string matattr = token[1];

    Material* material = mesh->getMaterial(matname);

    if(matattr == "texture")
    {
        int layer = token.size() >= 3 ? tools::strToNum<int>(token[2]) : 0;

        vector<string> valtoken = tools::tokenize(value, ';');

        Texture tex;
        tex.load(tools::pathScope(m_filename, valtoken[0], true), true);

        material->setTexture(tex, layer);

        if(valtoken.size() > 1)
        {
            if(valtoken[1] == "additive")
                material->setTextureBlend(Material::ADDITIVE, layer);
            else if(valtoken[1] == "modulate")
                material->setTextureBlend(Material::MODULATE, layer);
            else if(valtoken[1] == "replace")
                material->setTextureBlend(Material::REPLACE, layer);
        }

        if(valtoken.size() > 2)
            material->setTextureClipped(tools::strToNum<bool>(valtoken[2]), layer);

        if(valtoken.size() > 3)
            material->setTextureAnimation(tools::strToNum<unsigned >(valtoken[3]), layer);

        if(valtoken.size() > 4)
            material->setTextureFrameSize(Vector2i().fromStr(valtoken[4]), layer);

        if(valtoken.size() > 5)
            material->setTexturePart(Vector2i().fromStr(valtoken[5]), layer);
    }

    else
    {
        if(matattr == "cullTrick")
            toogleMaterial(material, Material::VERTEX_SORT_CULL_TRICK, value);
        if(matattr == "alphaThershold")
            material->setAlphaThershold(tools::strToNum<float>(value));
        if(matattr == "color")
            material->setColor(Vector4f().fromStr(value));
        if(matattr == "blendMod")
            toogleMaterial(material, Material::BLEND_MOD, value);
        if(matattr == "blendMul")
            toogleMaterial(material, Material::BLEND_MUL, value);
        if(matattr == "blendAdd")
            toogleMaterial(material, Material::BLEND_ADD, value);
        if(matattr == "alpha")
            toogleMaterial(material, Material::ALPHA, value);
        if(matattr == "frontFaceCull")
            toogleMaterial(material, Material::FRONTFACE_CULL, value);
        if(matattr == "backFaceCull")
            toogleMaterial(material, Material::BACKFACE_CULL, value);
        if(matattr == "lighted")
            toogleMaterial(material, Material::LIGHTED, value);
        if(matattr == "textured")
            toogleMaterial(material, Material::TEXTURED, value);
        if(matattr == "colored")
            toogleMaterial(material, Material::COLORED, value);
        if(matattr == "foged")
            toogleMaterial(material, Material::FOGED, value);
    }
}

inline void buildInherited(AbstractParser::Relation& rel, Node* parent, Node* current)
{
    parent->addChild(current);

    if(rel.attr.count("name"))
        current->setName(rel.attr["name"]);

    if(rel.attr.count("matrix"))
        current->setMatrix(rel.attr["matrix"]);

    for(AbstractParser::AttribMap::iterator it = rel.attr.begin(); it != rel.attr.end(); it++)
    {
        if(it->first[0] == '.')
        {
            string key(it->first, 1);
            current->setUserData(key, it->second);
        }
    }

    current->setup();
}

Node* AbstractParser::buildNode(Relation& rel, Node* parent)
{
    const string& iclass = rel.attr["class"];

    Node* current = NULL;

    if(iclass == "OBJMesh")
    {
        Mesh* mesh = m_classFactory ? m_classFactory->newMesh(m_meshScene) : new Mesh(m_meshScene);

        string modelFilepath;

        if(tools::isAbsoloutPath(rel.attr["filename"]))
            modelFilepath = rel.attr["filename"];
        else
            modelFilepath = tools::pathScope(m_filename, rel.attr["filename"], true);

        Mesh* shared = Mesh::isSharedBuffer(modelFilepath);

        if(shared)
        {
            mesh->sahreFrom(*shared);

            Mesh::registerBuffer(mesh, modelFilepath);
        }
        else
        {
            OBJMesh objfile(m_meshScene);
            objfile.open(modelFilepath);

            *mesh = objfile;

            Mesh::registerBuffer(mesh, modelFilepath);
        }

        if(rel.attr.count("billBoarding"))
            mesh->setBillBoard(Vector2b().fromStr(rel.attr["billBoarding"]));

        buildMaterial(rel.attr, mesh);

        mesh->addToConstructionMap("class", "OBJMesh");
        mesh->addToConstructionMap("filename", rel.attr["filename"]);

        mesh->computeAabb();

        buildInherited(rel, parent ? parent : m_rootNode, mesh);

        if(m_classFactory)
            m_classFactory->setupMesh(mesh);

        current = mesh;
    }

    else if(iclass == "ParticlesEmiter")
    {
        ParticlesEmiter* emiter = m_classFactory ? m_classFactory->newParticles(m_particlesScene) : new ParticlesEmiter(m_particlesScene);

        string modelFilepath;

        if(rel.attr.count("texture") && !rel.attr["texture"].empty())
        {
            if(rel.attr["texture"].find(':') != string::npos)
                modelFilepath = rel.attr["texture"];
            else
                modelFilepath = tools::pathScope(m_filename, rel.attr["texture"], true);

            emiter->setTexture(modelFilepath);
        }

        if(rel.attr.count("lifeInit"))
            emiter->setLifeInit(tools::strToNum<float>(rel.attr["lifeInit"]));
        if(rel.attr.count("lifeDown"))
            emiter->setLifeDown(tools::strToNum<float>(rel.attr["lifeDown"]));

        if(rel.attr.count("gravity"))
            emiter->setGravity(Vector3f().fromStr(rel.attr["gravity"]));
        if(rel.attr.count("boxSize"))
            emiter->setBoxSize(Vector3f().fromStr(rel.attr["boxSize"]));
        if(rel.attr.count("bulletSize"))
            emiter->setBulletSize(Vector2f().fromStr(rel.attr["bulletSize"]));

        if(rel.attr.count("number"))
            emiter->setNumber(tools::strToNum<int>(rel.attr["number"]));

        if(rel.attr.count("freeMove"))
            emiter->setFreeMove(tools::strToNum<float>(rel.attr["freeMove"]));

        if(rel.attr.count("continousMode"))
            emiter->setContinousMode(tools::strToNum<bool>(rel.attr["continousMode"]));
        if(rel.attr.count("usePointSprite"))
            emiter->setUsePointSprite(tools::strToNum<bool>(rel.attr["usePointSprite"]));

        emiter->build();

        buildInherited(rel, parent ? parent : m_rootNode, emiter);

        if(m_classFactory)
            m_classFactory->setupParticles(emiter);

        current = emiter;
    }

    else if(iclass == "Light")
    {
        Light* light = m_classFactory ? m_classFactory->newLight(m_lightScene) : new Light(m_lightScene);

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
        {
            delete light;
            throw tbe::Exception("SceneParser::ParseNode; Unknown light type (%s:%d)", m_filename.c_str(), m_parseLine);
        }

        light->setAmbient(Vector4f().fromStr(rel.attr["ambient"]));
        light->setDiffuse(Vector4f().fromStr(rel.attr["diffuse"]));
        light->setSpecular(Vector4f().fromStr(rel.attr["specular"]));

        buildInherited(rel, parent ? parent : m_rootNode, light);

        if(m_classFactory)
            m_classFactory->setupLight(light);

        current = light;
    }

    else if(iclass == "MapMark")
    {
        MapMark* mark = m_classFactory ? m_classFactory->newMapMark(m_markScene) : new MapMark(m_markScene);

        buildInherited(rel, parent ? parent : m_rootNode, mark);

        if(m_classFactory)
            m_classFactory->setupMapMark(mark);

        current = mark;
    }

    else
        throw Exception("SceneParser::parseNode; Unknown class (%s:%d)", m_filename.c_str(), m_parseLine);

    for(unsigned i = 0; i < rel.child.size(); i++)
        buildNode(rel.child[i], current);

    return current;
}

ParserClassFactory::ParserClassFactory()
{
}

ParserClassFactory::~ParserClassFactory()
{
}
