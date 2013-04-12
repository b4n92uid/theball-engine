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
#include "MapMark.h"
#include "ObjMesh.h"
#include "Ball3DMesh.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

SceneParser::SceneParser() { }

SceneParser::SceneParser(SceneManager* sceneManager) : AbstractParser(sceneManager) { }

SceneParser::~SceneParser() { }

void SceneParser::prepare()
{
    m_scheme.clear();

    m_scheme.put("scene", m_sceneName);
    m_scheme.put("scene.author", m_authorName);
    
    m_scheme.put_child("scene.attributes", m_attributes);

    Fog* fog = m_sceneManager->getFog();

    if(fog->isEnable())
    {
        //    m_scheme.put("scene.fog", fog->isEnable());
        m_scheme.put("scene.fog.color", fog->getColor());
        m_scheme.put("scene.fog.start", fog->getStart());
        m_scheme.put("scene.fog.end", fog->getEnd());
    }

    SkyBox* sky = m_sceneManager->getSkybox();

    if(sky->isEnable())
    {
        Texture* skytex = sky->getTextures();
        //    m_scheme.put("scene.skybox", sky->isEnable());
        m_scheme.put("scene.skybox.front", relativize(skytex[0].getFilename()));
        m_scheme.put("scene.skybox.back", relativize(skytex[1].getFilename()));
        m_scheme.put("scene.skybox.top", relativize(skytex[2].getFilename()));
        m_scheme.put("scene.skybox.bottom", relativize(skytex[3].getFilename()));
        m_scheme.put("scene.skybox.left", relativize(skytex[4].getFilename()));
        m_scheme.put("scene.skybox.right", relativize(skytex[5].getFilename()));
    }

    Shader rshade = m_meshScene->getRenderingShader();

    if(rshade.isEnable())
    {
        m_scheme.put("scene.shader.vertex", relativize(rshade.getVertFilename()));
        m_scheme.put("scene.shader.fragment", relativize(rshade.getFragFilename()));

        rtree bindtree;

        const Shader::UniformMap& umap = rshade.getRequestedUniform();

        BOOST_FOREACH(Shader::UniformMap::value_type v, umap)
        {
            bindtree.put(v.first, v.second);
        }

        m_scheme.put_child("scene.shader.bind", bindtree);
    }

    m_scheme.put("scene.ambiante", m_sceneManager->getAmbientLight());
    m_scheme.put("scene.znear", m_sceneManager->getZNear());
    m_scheme.put("scene.zfar", m_sceneManager->getZFar());

    for(Iterator<Node*> it = m_rootNode->getChildIterator(); it; it++)
    {
        rtree data = it->serialize(m_filename);

        m_scheme.add_child("content.node", data);
    }
}

void SceneParser::save()
{
    if(m_filename.empty())
        return;

    save(m_filename);
}

void SceneParser::save(const std::string& filepath)
{
    boost::property_tree::write_info(filepath, m_scheme);
}

void SceneParser::clear()
{
    AbstractParser::clear();
}

void SceneParser::load(const std::string& filepath)
{
    clear();

    boost::property_tree::read_info(filepath, m_scheme);

    if(!m_scheme.count("scene") || !m_scheme.count("content"))
        throw Exception("SceneParser::load; Invalid scene format (%s)", filepath.c_str());

    m_sceneName = m_scheme.get<string>("scene");
    m_authorName = m_scheme.get<string>("scene.author");
    m_attributes = m_scheme.get_child("scene.attributes");

    m_filename = filepath;
}

void SceneParser::build()
{
    VectorTranslator<Vector4f> v4ftr;

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

    if(m_scheme.get_child("scene").count("fog"))
    {
        scene::Fog* fog = m_sceneManager->getFog();
        fog->setEnable(m_scheme.get<bool>("scene.fog", false));
        fog->setColor(m_scheme.get<Vector4f>("scene.fog.color", v4ftr));
        fog->setStart(m_scheme.get<float>("scene.fog.start"));
        fog->setEnd(m_scheme.get<float>("scene.fog.end"));
    }

    if(m_scheme.get_child("scene").count("skybox"))
    {
        TextureTranslator textr;

        Texture skytex[6] = {
            resolve(m_scheme.get<string>("scene.skybox.front")),
            resolve(m_scheme.get<string>("scene.skybox.back")),
            resolve(m_scheme.get<string>("scene.skybox.top")),
            resolve(m_scheme.get<string>("scene.skybox.bottom")),
            resolve(m_scheme.get<string>("scene.skybox.left")),
            resolve(m_scheme.get<string>("scene.skybox.right")),
        };

        scene::SkyBox * sky = m_sceneManager->getSkybox();
        sky->setTextures(skytex);
        sky->setEnable(true);
    }

    if(m_scheme.get_child("scene").count("shader"))
    {
        Shader shader;

        if(m_scheme.get_child("scene.shader").count("vertex"))
        {
            string path = resolve(m_scheme.get<string>("scene.shader.vertex"));
            shader.loadVertexShader(path);
        }

        if(m_scheme.get_child("scene.shader").count("fragment"))
        {
            string path = resolve(m_scheme.get<string>("scene.shader.fragment"));
            shader.loadFragmentShader(path);
        }

        shader.loadProgram();

        if(m_scheme.get_child("scene.shader").count("bind"))
            BOOST_FOREACH(rtree::value_type & b, m_scheme.get_child("scene.shader.bind"))
        {
            shader.setRequestedUniform(b.first, b.second.data());
        }


        m_meshScene->setRenderingShader(shader);
    }

    m_sceneManager->setAmbientLight(m_scheme.get<Vector4f>("scene.ambient", v4ftr));

    m_sceneManager->setZNear(m_scheme.get<float>("scene.znear"));
    m_sceneManager->setZFar(m_scheme.get<float>("scene.zfar"));
    m_sceneManager->updateViewParameter();

    BOOST_FOREACH(rtree::value_type &v, m_scheme.get_child("content"))
    {
        buildNode(v.second, m_rootNode);
    }
}

void SceneParser::setSceneName(std::string sceneName)
{
    m_sceneName = sceneName;
}

std::string SceneParser::getSceneName() const
{
    return m_sceneName;
}

void SceneParser::setAuthorName(std::string authorName)
{
    m_authorName = authorName;
}

std::string SceneParser::getAuthorName() const
{
    return m_authorName;
}

rtree& SceneParser::attributes()
{
    return m_attributes;
}

const rtree& SceneParser::scheme()
{
    return m_scheme;
}
