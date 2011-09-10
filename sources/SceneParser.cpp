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
#include "Water.h"
#include "MapMark.h"
#include "ObjMesh.h"
#include "Ball3DMesh.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

ParserClassFactory::ParserClassFactory()
{
}

ParserClassFactory::~ParserClassFactory()
{
}

ParserHandle::ParserHandle()
{
    m_sceneManager = NULL;
    m_classFactory = NULL;
    m_lightScene = NULL;
    m_meshScene = NULL;
    m_particlesScene = NULL;
    m_waterScene = NULL;
    m_markScene = NULL;
    m_version = 0.1;
}

ParserHandle::ParserHandle(SceneManager* sceneManager)
{
    m_sceneManager = sceneManager;
    m_classFactory = NULL;
    m_lightScene = NULL;
    m_meshScene = NULL;
    m_particlesScene = NULL;
    m_waterScene = NULL;
    m_markScene = NULL;
    m_version = 0.1;
}

ParserHandle::~ParserHandle()
{
    delete m_classFactory;
}

ParticlesParallelScene* ParserHandle::getParticlesScene() const
{
    return m_particlesScene;
}

MeshParallelScene* ParserHandle::getMeshScene() const
{
    return m_meshScene;
}

WaterParallelScene* ParserHandle::getWaterScene() const
{
    return m_waterScene;
}

LightParallelScene* ParserHandle::getLightScene() const
{
    return m_lightScene;
}

MapMarkParallelScene* ParserHandle::getMarkScene() const
{
    return m_markScene;
}

void ParserHandle::setClassFactory(ParserClassFactory* classFactory)
{
    this->m_classFactory = classFactory;
}

ParserClassFactory* ParserHandle::getClassFactory() const
{
    return m_classFactory;
}

void ParserHandle::setWaterScene(WaterParallelScene* waterScene)
{
    this->m_waterScene = waterScene;
}

void ParserHandle::setParticlesScene(ParticlesParallelScene* particlesScene)
{
    this->m_particlesScene = particlesScene;
}

void ParserHandle::setMeshScene(MeshParallelScene* meshScene)
{
    this->m_meshScene = meshScene;
}

void ParserHandle::setLightScene(LightParallelScene* lightScene)
{
    this->m_lightScene = lightScene;
}

void ParserHandle::setMarkScene(MapMarkParallelScene* markScene)
{
    this->m_markScene = markScene;
}

SceneParser::SceneParser() :
m_rootNode(NULL)
{
}

SceneParser::SceneParser(SceneManager* sceneManager) :
ParserHandle(sceneManager), m_rootNode(sceneManager->getRootNode())
{
}

SceneParser::~SceneParser()
{
}

void SceneParser::prepareNodeConstruction(Node* node, Relation& rel)
{
    if(tools::find(m_excludedNodes, node))
        return;

    rel.attr = node->constructionMap(m_mapDescriptor.fileName);
    rel.deep = node->deepPosition() - 1;

    for(Iterator<Node*> it = node->getChildIterator(); it; it++)
    {
        Relation subrel;
        prepareNodeConstruction(*it, subrel);

        rel.child.push_back(subrel);
    }
}

void SceneParser::outpuNodeConstruction(Relation& rel, std::ofstream& file)
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

SceneParser& SceneParser::exclude(Node *node)
{
    m_excludedNodes.push_back(node);
    return *this;
}

void SceneParser::prepareScene()
{
    Fog* fog = m_sceneManager->getFog();

    m_mapDescriptor.fog.enable = fog->isEnable();
    m_mapDescriptor.fog.color = fog->getColor();
    m_mapDescriptor.fog.end = fog->getEnd();
    m_mapDescriptor.fog.start = fog->getStart();

    SkyBox* sky = m_sceneManager->getSkybox();
    Texture* skytex = sky->getTextures();

    m_mapDescriptor.skybox.enable = sky->isEnable();
    m_mapDescriptor.skybox.front = tools::pathScope(m_mapDescriptor.fileName, skytex[0].getFilename(), false);
    m_mapDescriptor.skybox.back = tools::pathScope(m_mapDescriptor.fileName, skytex[1].getFilename(), false);
    m_mapDescriptor.skybox.top = tools::pathScope(m_mapDescriptor.fileName, skytex[2].getFilename(), false);
    m_mapDescriptor.skybox.bottom = tools::pathScope(m_mapDescriptor.fileName, skytex[3].getFilename(), false);
    m_mapDescriptor.skybox.left = tools::pathScope(m_mapDescriptor.fileName, skytex[4].getFilename(), false);
    m_mapDescriptor.skybox.right = tools::pathScope(m_mapDescriptor.fileName, skytex[5].getFilename(), false);

    m_mapDescriptor.ambiante = m_sceneManager->getAmbientLight();

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

void SceneParser::saveScene()
{
    if(m_mapDescriptor.fileName.empty())
        return;

    saveScene(m_mapDescriptor.fileName);
}

void SceneParser::saveScene(const std::string& filepath)
{
    ofstream file(filepath.c_str());

    if(!file)
        throw tbe::Exception("SceneParser::SaveScene; Open file error (%s)", filepath.c_str());

    m_mapDescriptor.fileName = filepath;

    file << "v0.1 scene" << endl;
    file << endl;

    file << "*general" << endl;
    file << "name=" << m_mapDescriptor.sceneName << endl;
    file << "author=" << m_mapDescriptor.authorName << endl;
    file << "ambient=" << m_mapDescriptor.ambiante << endl;

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

    for(unsigned i = 0; i < m_mapDescriptor.nodes.size(); i++)
        outpuNodeConstruction(m_mapDescriptor.nodes[i], file);

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

    m_mapDescriptor.fileName = filepath;
    m_mapDescriptor.sceneName.clear();
    m_mapDescriptor.authorName.clear();
    m_mapDescriptor.ambiante = 0.2;
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
    m_mapDescriptor.nodes.clear();

    string buffer;

    tools::getline(file, buffer);

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
            m_mapDescriptor.nodes.push_back(rel);
        }

        else if(buffer.substr(0, 8) == ".include")
        {
            string modelFilepath(buffer, 9);

            if(modelFilepath.find(':') == string::npos)
                modelFilepath = tools::pathScope(m_mapDescriptor.fileName, modelFilepath, true);

            loadScene(modelFilepath);
        }

        else
            throw tbe::Exception("SceneParser::LoadScene; Parse error %d (%s)", line, buffer.c_str());
    }

    file.close();
}

void SceneParser::parseGeneral(AttribMap& att)
{
    m_mapDescriptor.sceneName = att["name"];
    m_mapDescriptor.authorName = att["author"];
    m_mapDescriptor.ambiante.fromStr(att["ambient"]);
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
        m_mapDescriptor.skybox.front = tools::pathScope(m_mapDescriptor.fileName, att["front"], true);

    if(!att["back"].empty())
        m_mapDescriptor.skybox.back = tools::pathScope(m_mapDescriptor.fileName, att["back"], true);

    if(!att["top"].empty())
        m_mapDescriptor.skybox.top = tools::pathScope(m_mapDescriptor.fileName, att["top"], true);

    if(!att["bottom"].empty())
        m_mapDescriptor.skybox.bottom = tools::pathScope(m_mapDescriptor.fileName, att["bottom"], true);

    if(!att["left"].empty())
        m_mapDescriptor.skybox.left = tools::pathScope(m_mapDescriptor.fileName, att["left"], true);

    if(!att["right"].empty())
        m_mapDescriptor.skybox.right = tools::pathScope(m_mapDescriptor.fileName, att["right"], true);

    m_mapDescriptor.skybox.enable = true;
}

inline void toogleMaterial(Material* material, int mod, string stat)
{
    tools::strToNum<bool>(stat)
            ? material->enable(mod)
            : material->disable(mod);
}

void SceneParser::buildScene()
{
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

    m_sceneManager->setAmbientLight(m_mapDescriptor.ambiante);

    for(unsigned i = 0; i < m_mapDescriptor.nodes.size(); i++)
    {
        buildNode(m_mapDescriptor.nodes[i]);
    }
}

void SceneParser::buildMaterial(AttribMap& att, Mesh* mesh)
{
    mesh->setOutputMaterial(false);

    for(AttribMap::iterator it = att.begin(); it != att.end(); it++)
    {
        string key = it->first;

        if(key[0] == '!')
        {
            mesh->setOutputMaterial(true);

            key.erase(0, 1);

            vector<string> token = tools::tokenize(key, ':');

            if(token.empty() || token.size() < 2)
                continue;

            string matname = token[0];
            string attributs = token[1];

            Material* material = mesh->getMaterial(matname);

            if(attributs == "texture")
            {
                vector<string> valtoken = tools::tokenize(it->second, ';');

                Texture tex;
                tex.load(tools::pathScope(m_mapDescriptor.fileName, valtoken[0], true), true);

                int layer = tools::strToNum<int>(token[2]);

                material->setTexture(tex, layer);

                if(valtoken[1] == "additive")
                    material->setTextureBlend(Material::ADDITIVE, layer);
                if(valtoken[1] == "modulate")
                    material->setTextureBlend(Material::MODULATE, layer);
                if(valtoken[1] == "replace")
                    material->setTextureBlend(Material::REPLACE, layer);

            }

            else
            {
                if(attributs == "cullTrick")
                    toogleMaterial(material, Material::VERTEX_SORT_CULL_TRICK, it->second);
                if(attributs == "alphaThershold")
                    material->setAlphaThershold(tools::strToNum<float>(it->second));
                if(attributs == "blendMod")
                    toogleMaterial(material, Material::BLEND_MOD, it->second);
                if(attributs == "blendMul")
                    toogleMaterial(material, Material::BLEND_MUL, it->second);
                if(attributs == "blendAdd")
                    toogleMaterial(material, Material::BLEND_ADD, it->second);
                if(attributs == "alpha")
                    toogleMaterial(material, Material::ALPHA, it->second);
                if(attributs == "frontFaceCull")
                    toogleMaterial(material, Material::FRONTFACE_CULL, it->second);
                if(attributs == "backFaceCull")
                    toogleMaterial(material, Material::BACKFACE_CULL, it->second);
                if(attributs == "lighted")
                    toogleMaterial(material, Material::LIGHTED, it->second);
                if(attributs == "textured")
                    toogleMaterial(material, Material::TEXTURED, it->second);
                if(attributs == "colored")
                    toogleMaterial(material, Material::COLORED, it->second);
                if(attributs == "foged")
                    toogleMaterial(material, Material::FOGED, it->second);
            }
        }
    }
}

inline void buildInherited(SceneParser::Relation& rel, Node* parent, Node* current)
{
    parent->addChild(current);

    if(rel.attr.count("name"))
        current->setName(rel.attr["name"]);

    if(rel.attr.count("matrix"))
        current->setMatrix(rel.attr["matrix"]);

    for(SceneParser::AttribMap::iterator it = rel.attr.begin(); it != rel.attr.end(); it++)
    {
        if(it->first[0] == '.')
        {
            string key(it->first, 1);
            current->setUserData(key, it->second);
        }
    }
}

void SceneParser::buildNode(Relation& rel, Node* parent)
{
    const string& iclass = rel.attr["class"];

    Node* current = NULL;

    if(iclass == "OBJMesh")
    {
        OBJMesh objfile(m_meshScene);

        string modelFilepath;

        if(rel.attr["filename"].find(':') != string::npos)
            modelFilepath = rel.attr["filename"];
        else
            modelFilepath = tools::pathScope(m_mapDescriptor.fileName, rel.attr["filename"], true);

        objfile.open(modelFilepath);

        Mesh* mesh = m_classFactory ? m_classFactory->newMesh(m_meshScene) : new Mesh(m_meshScene);

        *mesh = objfile;

        if(rel.attr.count("vertexScale"))
            mesh->setVertexScale(Vector3f().fromStr(rel.attr["vertexScale"]));
        if(rel.attr.count("color"))
            mesh->setColor(Vector4f().fromStr(rel.attr["color"]));
        if(rel.attr.count("opacity"))
            mesh->setOpacity(tools::strToNum<float>(rel.attr["opacity"]));
        if(rel.attr.count("billBoarding"))
            mesh->setBillBoard(Vector2b().fromStr(rel.attr["billBoarding"]));

        buildMaterial(rel.attr, mesh);

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
                modelFilepath = tools::pathScope(m_mapDescriptor.fileName, rel.attr["texture"], true);

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
            throw tbe::Exception("SceneParser::ParseNode; Unknown light type (%s)", rel.attr["type"].c_str());
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
        throw Exception("SceneParser::parseNode; Unknown class (%s)", iclass.c_str());

    for(unsigned i = 0; i < rel.child.size(); i++)
        buildNode(rel.child[i], current);
}

const SceneParser::AttribMap SceneParser::additionalFields() const
{
    return m_additional;
}

SceneParser::MapDescriptor& SceneParser::getMapDescriptor()
{
    return m_mapDescriptor;
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

std::string SceneParser::getAdditionalString(std::string key)
{
    if(m_additional.count(key))
        return m_additional[key];
    else
        return std::string();
}

void SceneParser::setAdditionalString(std::string key, std::string value)
{
    m_additional[key] = value;
}

SceneParser::MapDescriptor::MapDescriptor()
{
    fog.start = 32;
    fog.end = 64;
    fog.enable = false;

    skybox.enable = false;
}

ClassParser::ClassParser()
{
    m_buildedNod = NULL;
}

ClassParser::ClassParser(SceneManager* sceneManager) : ParserHandle(sceneManager)
{
    m_buildedNod = NULL;
}

ClassParser::~ClassParser()
{
}

void ClassParser::loadClass(const std::string& path)
{
}

void ClassParser::buildClass()
{
}

void ClassParser::prepareClass()
{
}

void ClassParser::saveClass()
{
}

void ClassParser::saveClass(const std::string& path)
{
}

void ClassParser::setBuildedNod(Node* buildedNod)
{
    this->m_buildedNod = buildedNod;
}

Node* ClassParser::getBuildedNod() const
{
    return m_buildedNod;
}
