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
#include "ShadowMap.h"
#include "VolumetricLight.h"
#include "Water.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;
using namespace boost::filesystem;
using namespace boost::property_tree;

AbstractParser::AbstractParser()
{
    m_sceneManager = NULL;
    m_rootNode = NULL;
    m_classFactory = NULL;
    m_meshScene = NULL;
    m_particlesScene = NULL;
    m_markScene = NULL;
    m_waterScene = NULL;
}

AbstractParser::AbstractParser(SceneManager* sceneManager)
{
    m_sceneManager = sceneManager;
    m_rootNode = sceneManager->getRootNode();
    m_classFactory = NULL;
    m_meshScene = NULL;
    m_particlesScene = NULL;
    m_markScene = NULL;
    m_waterScene = NULL;
}

AbstractParser::~AbstractParser()
{
    delete m_classFactory;
}

void AbstractParser::setFilename(std::string filename)
{
    m_filename = filename;
}

std::string AbstractParser::getFilename() const
{
    return m_filename;
}

void AbstractParser::clear()
{
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

MapMarkParallelScene* AbstractParser::getMarkScene() const
{
    return m_markScene;
}

void AbstractParser::setWaterScene(WaterParallelScene* waterScene)
{
    this->m_waterScene = waterScene;
}

WaterParallelScene* AbstractParser::getWaterScene() const
{
    return m_waterScene;
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

void AbstractParser::setMarkScene(MapMarkParallelScene* markScene)
{
    this->m_markScene = markScene;
}

std::string AbstractParser::resolve(std::string relpath, std::string base)
{
    if(tools::isAbsoloutPath(relpath))
        return relpath;

    if(base.empty())
        base = m_filename;

    fspath output = fspath(base).remove_filename();

    output /= relpath;

    return output.normalize().string();
}

std::string AbstractParser::relativize(std::string abspath)
{
    if(!tools::isAbsoloutPath(abspath))
        return abspath;

    return tools::relativizePath(abspath, m_filename);
}

void AbstractParser::buildInherited(rtree data, Node* parent, Node* current)
{
    Matrix4Translator m4tr;

    if(parent)
        parent->addChild(current);

    if(data.count("name"))
        current->setName(data.get<string>("name"));

    if(data.count("matrix"))
        current->setMatrix(data.get<Matrix4>("matrix", m4tr));

    if(data.count("position"))
        current->setPos(data.get<Vector3f>("position"));

    if(data.count("scale"))
        current->setScale(data.get<Vector3f>("scale"));

    if(data.count("rotation"))
        current->setRotation(data.get<Quaternion>("rotation"));

    if(data.count("attributes")) BOOST_FOREACH(rtree::value_type & v, data.get_child("attributes"))
    {
        current->setUserData(v.first, v.second.data());
    }

    current->setup();
}

Node* AbstractParser::buildNode(rtree data, Node* parent)
{
    const string iclass = data.get<string>("class");

    Node* current = NULL;

    if(iclass == "Mesh")
    {
        Mesh* mesh = m_classFactory ? m_classFactory->newMesh(m_meshScene) : new Mesh(m_meshScene);

        string path = data.get<string>("class.path");

        mesh->serializing().put("class.path", tools::relativizePath(path, m_filename));
        mesh->serializing().put("class.format", "obj");

        if(!tools::isAbsoloutPath(path))
            path = resolve(path);

        Mesh* shared = Mesh::isSharedBuffer(path);

        if(shared)
        {
            mesh->shareVertexes(*shared);
        }
        else
        {
            OBJMesh objfile(m_meshScene);
            objfile.open(path);

            mesh->fetchVertexes(objfile);
        }

        Mesh::registerBuffer(mesh, path);

        mesh->setBillBoard(data.get<Vector2b>("class.billBoarding", Vector2b(false)));
        mesh->setReceiveShadow(data.get<bool>("class.receiveShadow", true));
        mesh->setCastShadow(data.get<bool>("class.castShadow", true));

        if(data.get<bool>("class.computeNormal", false))
            mesh->computeNormal();

        if(data.get<bool>("class.computeTangent", false))
            mesh->computeTangent();

        if(data.get<bool>("class.computeAocc", false))
            mesh->computeAocc();

        if(data.count("material"))
        {
            rtree material = data.get_child("material");

            BOOST_FOREACH(rtree::value_type& v, material)
            {
                SubMesh* smesh = mesh->getSubMesh(v.first);

                if(!smesh)
                {
                    cout << "/!\\ WARNING; AbstractParser::buildNode; SubMesh " << v.first << " not found" << endl;
                    continue;
                }

                string path = v.second.get_value<string>();

                if(!tools::isAbsoloutPath(path))
                    path = resolve(path);

                Material* mat = MaterialManager::get()->loadMaterial(path);
                smesh->setMaterial(mat);

                string relpath = v.second.get_value<string>();
                mesh->serializing().put("material." + v.first, relpath);
            }
        }

        mesh->computeAabb();

        buildInherited(data, parent, mesh);

        if(m_classFactory)
            m_classFactory->setupMesh(mesh);

        current = mesh;
    }

    else if(iclass == "ParticlesEmiter")
    {
        ParticlesEmiter* emiter = m_classFactory ? m_classFactory->newParticles(m_particlesScene) : new ParticlesEmiter(m_particlesScene);

        string texturePath = data.get<string>("class.texture");
        emiter->setTexture(resolve(texturePath));

        emiter->setGravity(data.get<Vector3f>("class.gravity", Vector3f(0)));

        emiter->setBoxSize(data.get<Vector3f>("class.boxSize", Vector3f(0)));

        emiter->setBulletSize(data.get<Vector2f>("class.bulletSize", Vector2f(0.5)));

        emiter->setLifeInit(data.get<float>("class.lifeInit", 1));
        emiter->setLifeDown(data.get<float>("class.lifeDown", 0.1));

        emiter->setNumber(data.get<int>("class.number", 128));

        emiter->setFreeMove(data.get<float>("class.freeMove", 0));

        emiter->setContinousMode(data.get<bool>("class.continousMode", true));
        emiter->setUsePointSprite(data.get<bool>("class.usePointSprite", true));

        emiter->build();

        buildInherited(data, parent, emiter);

        if(m_classFactory)
            m_classFactory->setupParticles(emiter);

        current = emiter;
    }

    else if(iclass == "Light")
    {
        rtree rclass = data.get_child("class");

        Light* light = m_classFactory ? m_classFactory->newLight(m_meshScene) : new Light(m_meshScene);

        string type = rclass.get<string>("type");

        if(type == "diri")
        {
            light->setType(Light::DIRI);
        }

        else if(type == "point")
        {
            light->setType(Light::POINT);
            light->setRadius(rclass.get<float>("radius", 1.0));
        }

        else
        {
            delete light;
            throw tbe::Exception("SceneParser::ParseNode; Unknown light type (%1%)") % type;
        }

        VectorTranslator<Vector4f> v4tr;

        light->setAmbient(rclass.get<Vector4f>("ambient", Vector4f(0), v4tr));
        light->setDiffuse(rclass.get<Vector4f>("diffuse", Vector4f(1), v4tr));
        light->setSpecular(rclass.get<Vector4f>("specular", Vector4f(0), v4tr));
        light->setCastShadow(rclass.get<bool>("castShadow", true));
        light->setCastRays(rclass.get<bool>("castRays", false));

        if(rclass.count("shadowMap"))
        {
            ShadowMap* smap = new ShadowMap(light);

            smap->setFrameSize(rclass.get<Vector2i>("shadowMap.size", Vector2i(512)));
            smap->setBlurPass(rclass.get<int>("shadowMap.blur", 0));
            smap->setIntensity(rclass.get<float>("shadowMap.intensity", 0.5));
            smap->setShaderHandled(rclass.get<bool>("shadowMap.shader", true));

            light->setShadowMap(smap);
        }

        if(rclass.count("volumeLight"))
        {
            VolumetricLight* vlight = new VolumetricLight(light);
            vlight->setFrameSize(rclass.get<Vector2i>("volumeLight.frameSize", Vector2i(512)));
            vlight->setLightSize(rclass.get<Vector2f>("volumeLight.lightSize", Vector2f(64)));
            vlight->setOffset(rclass.get<Vector3f>("volumeLight.offset", Vector3f(0)));

            boost::optional<string> noiselayer = rclass.get_optional<string>("volumeLight.noiseLayer");

            if(noiselayer)
                vlight->setNoiseLayer(resolve(*noiselayer));

            light->setVolumeLight(vlight);
        }

        buildInherited(data, parent, light);

        if(m_classFactory)
            m_classFactory->setupLight(light);

        current = light;
    }

    else if(iclass == "MapMark")
    {
        MapMark* mark = m_classFactory ? m_classFactory->newMapMark(m_markScene) : new MapMark(m_markScene);

        buildInherited(data, parent, mark);

        if(m_classFactory)
            m_classFactory->setupMapMark(mark);

        current = mark;
    }

    else if(iclass == "Water")
    {
        Water* wat = new Water(m_waterScene);

        rtree rclass = data.get_child("class");

        wat->setSize(rclass.get<Vector2f>("size"));
        wat->setSpeed(rclass.get<float>("speed"));
        wat->setDeform(rclass.get<float>("deform"));
        wat->setBlend(rclass.get<float>("blend"));
        wat->setUvRepeat(rclass.get<Vector2f>("uvscale"));

        string path = rclass.get<string>("normalMap");
        wat->setNormalMap(Texture(resolve(path), true));

        buildInherited(data, parent, wat);

        current = wat;
    }

    else
    {
        string classfile = iclass;

        if(!tools::isAbsoloutPath(classfile))
            classfile = resolve(classfile);

        if(exists(classfile))
        {
            rtree nodetree;
            read_info(classfile, nodetree);

            current = buildNode(nodetree.get_child("Class"), parent);
            current->serializing().put("class", iclass);

            buildInherited(data, NULL, current);
        }

        else
            throw Exception("AbstractParser::parseNode; Unknown class (%1%)") % iclass;
    }

    if(data.count("childs")) BOOST_FOREACH(rtree::value_type & v, data.get_child("childs"))
    {
        buildNode(v.second, current);
    }

    return current;
}

ParserClassFactory::ParserClassFactory() { }

ParserClassFactory::~ParserClassFactory() { }
