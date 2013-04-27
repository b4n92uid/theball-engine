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
}

AbstractParser::AbstractParser(SceneManager* sceneManager)
{
    m_sceneManager = sceneManager;
    m_rootNode = sceneManager->getRootNode();
    m_classFactory = NULL;
    m_meshScene = NULL;
    m_particlesScene = NULL;
    m_markScene = NULL;
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
    if(base.empty())
        base = m_filename;

    fspath output = fspath(base).remove_filename();

    output /= relpath;

    return output.normalize().string();
}

std::string AbstractParser::relativize(std::string parh)
{
    return tools::relativizePath(parh, m_filename);
}

void AbstractParser::buildShader(rtree data, Material* mat)
{
    if(!data.count("shader"))
        return;

    Shader shader;

    if(data.get_optional<string>("shader.vertex"))
    {
        string path = data.get<string>("shader.vertex");

        if(!tools::isAbsoloutPath(path))
            path = resolve(path);

        shader.loadVertexShader(path);
    }

    if(data.get_optional<string>("shader.fragment"))
    {
        string path = data.get<string>("shader.fragment");

        if(!tools::isAbsoloutPath(path))
            path = resolve(path);

        shader.loadFragmentShader(path);
    }

    shader.loadProgram();

    if(data.get_child("shader").count("bind"))
        BOOST_FOREACH(rtree::value_type & b, data.get_child("shader.bind"))
    {
        shader.setRequestedUniform(b.first, b.second.data());
    }

    mat->setShader(shader);
}

void AbstractParser::buildMaterial(rtree data, Mesh* mesh)
{
    VectorTranslator<Vector4f> v4tr;

    mesh->setOutputMaterial(true);

    BOOST_FOREACH(rtree::value_type &v, data)
    {
        Material* mat = mesh->getMaterial(v.second.get_value<string>());

        mat->setAmbient(v.second.get<Vector4f>("ambient", Vector4f(1), v4tr));
        mat->setDiffuse(v.second.get<Vector4f>("diffuse", Vector4f(1), v4tr));
        mat->setSpecular(v.second.get<Vector4f>("specular", Vector4f(0.5), v4tr));
        mat->setShininess(v.second.get<float>("shininess", 16));

        if(v.second.get<bool>("alpha", false))
        {
            mat->enable(Material::ALPHA);
            mat->setAlphaThershold(v.second.get<float>("alphaThershold", 0.0f));
        }

        if(v.second.get<bool>("colored", true))
        {
            mat->enable(Material::COLORED);
            mat->setColor(v.second.get<Vector4f>("color", Vector4f(1, 1, 1, 1), VectorTranslator<Vector4f>()));
        }

        if(v.second.get<string>("blend", "none") != "none")
        {
            string blend = v.second.get<string>("blend");

            if(blend == "additive")
                mat->enable(Material::BLEND_ADD);

            else if(blend == "modulate")
                mat->enable(Material::BLEND_MOD);

            else if(blend == "multiply")
                mat->enable(Material::BLEND_MUL);
        }

        if(v.second.get<string>("faceCull", "none") != "none")
        {
            string cull = v.second.get<string>("faceCull");

            if(cull == "back")
                mat->enable(Material::BACKFACE_CULL);

            else if(cull == "front")
                mat->enable(Material::FRONTFACE_CULL);
        }

        if(v.second.get<bool>("lighted", true))
            mat->enable(Material::LIGHTED);
        else
            mat->disable(Material::LIGHTED);

        if(v.second.get<bool>("foged", true))
            mat->enable(Material::FOGED);
        else
            mat->disable(Material::FOGED);

        if(v.second.get<bool>("textured", true))
            mat->enable(Material::TEXTURED);
        else
            mat->disable(Material::TEXTURED);

        if(v.second.count("textures"))
        {
            rtree textures = v.second.get_child("textures");

            rtree::iterator it = textures.begin();
            for(int i = 0; it != textures.end(); i++)
            {
                string path = it->second.get<string>("path");

                if(!tools::isAbsoloutPath(path))
                    path = resolve(path);

                mat->setTexture(Texture(path, true), i);

                string blend = it->second.get<string>("blend", "modulate");

                if(blend == "modulate")
                    mat->setTextureBlend(Material::MODULATE, i);

                else if(blend == "additive")
                    mat->setTextureBlend(Material::ADDITIVE, i);

                else if(blend == "replace")
                    mat->setTextureBlend(Material::REPLACE, i);

                it++;
            }
        }

        if(v.second.count("shader"))
        {
            boost::optional<rtree&> inlineShaderTree = v.second.get_child_optional("shader");

            rtree shaderTree;

            if(inlineShaderTree)
            {
                shaderTree = *inlineShaderTree;
            }
            else
            {
                string path = v.second.get<string>("shader");

                // TODO Reslove relative to material file (if one)
                if(!tools::isAbsoloutPath(path))
                    path = resolve(path);

                boost::property_tree::read_info(path, shaderTree);
            }

            buildShader(shaderTree, mat);
        }
    }
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

        mesh->addSerializeValue("class.path", path);
        mesh->addSerializeValue("class.format", "obj");

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
        mesh->setCastShadow(data.get<bool>("class.castShadow(", true));

        if(data.count("material"))
        {
            boost::optional<rtree&> inlineMaTree = data.get_child_optional("material");

            rtree matTree;

            if(inlineMaTree)
            {
                matTree = *inlineMaTree;
            }
            else
            {
                string path = data.get<string>("material");

                mesh->addSerializeValue("material", path);

                if(!tools::isAbsoloutPath(path))
                    path = resolve(path);

                boost::property_tree::read_info(path, matTree);
            }

            buildMaterial(matTree, mesh);
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

        if(data.count("class.texture"))
        {
            rtree textures = data.get_child("class.texture");

            fspath path = textures.get<fspath>("path");

            emiter->setTexture(path.string());
        }

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
        Light* light = m_classFactory ? m_classFactory->newLight(m_meshScene) : new Light(m_meshScene);

        string type = data.get<string>("class.type");

        if(type == "diri")
        {
            light->setType(Light::DIRI);
        }

        else if(type == "point")
        {
            light->setType(Light::POINT);
            light->setRadius(data.get<float>("class.radius", 1.0));
        }

        else
        {
            delete light;
            throw tbe::Exception("SceneParser::ParseNode; Unknown light type (%s)", type.c_str());
        }

        VectorTranslator<Vector4f> v4tr;

        light->setAmbient(data.get<Vector4f>("class.ambient", Vector4f(0), v4tr));
        light->setDiffuse(data.get<Vector4f>("class.diffuse", Vector4f(1), v4tr));
        light->setSpecular(data.get<Vector4f>("class.specular", Vector4f(0), v4tr));
        light->setCastShadow(data.get<bool>("class.castShadow", true));

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
            current->addSerializeValue("class", iclass);
        }

        else
            throw Exception("AbstractParser::parseNode; Unknown class (%s)", iclass.c_str());
    }

    if(data.count("childs")) BOOST_FOREACH(rtree::value_type & v, data.get_child("childs"))
    {
        buildNode(v.second, current);
    }

    return current;
}

ParserClassFactory::ParserClassFactory() { }

ParserClassFactory::~ParserClassFactory() { }
