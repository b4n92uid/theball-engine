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
#include "ShadowMap.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

SceneParser::SceneParser() { }

SceneParser::SceneParser(SceneManager* sceneManager) : AbstractParser(sceneManager) { }

SceneParser::~SceneParser() { }

void SceneParser::prepare()
{
    m_scheme.clear();
    m_scheme = m_identity;

    m_scheme.put("Scene", m_sceneName);
    m_scheme.put("Scene.author", m_authorName);

    if(!m_attributes.empty())
        m_scheme.put_child("Scene.attributes", m_attributes);

    ShadowMap* smap = m_sceneManager->getShadowMap();

    {
        m_scheme.put("Scene.shadow", smap->isEnabled());
        m_scheme.put("Scene.shadow.size", smap->getFrameSize().toStr());
        m_scheme.put("Scene.shadow.blur", smap->getBlurPass());
        m_scheme.put("Scene.shadow.intensity", smap->getIntensity());
    }

    Fog* fog = m_sceneManager->getFog();

    {
        m_scheme.put("Scene.fog", fog->isEnable());
        m_scheme.put("Scene.fog.color", fog->getColor());
        m_scheme.put("Scene.fog.start", fog->getStart());
        m_scheme.put("Scene.fog.end", fog->getEnd());
    }

    SkyBox* sky = m_sceneManager->getSkybox();

    {
        Texture* skytex = sky->getTextures();

        m_scheme.put("Scene.skybox", sky->isEnable());
        m_scheme.put("Scene.skybox.front", relativize(skytex[0].getFilename()));
        m_scheme.put("Scene.skybox.back", relativize(skytex[1].getFilename()));
        m_scheme.put("Scene.skybox.top", relativize(skytex[2].getFilename()));
        m_scheme.put("Scene.skybox.bottom", relativize(skytex[3].getFilename()));
        m_scheme.put("Scene.skybox.left", relativize(skytex[4].getFilename()));
        m_scheme.put("Scene.skybox.right", relativize(skytex[5].getFilename()));
    }

    /* Handled by identity scheme
    
    Shader rshade = m_meshScene->getRenderingShader();

    if(rshade.isEnable())
    {
        m_scheme.put("Scene.shader.vertex", relativize(rshade.getVertFilename()));
        m_scheme.put("Scene.shader.fragment", relativize(rshade.getFragFilename()));

        rtree bindtree;

        const Shader::UniformMap& umap = rshade.getRequestedUniform();

        BOOST_FOREACH(Shader::UniformMap::value_type v, umap)
        {
            bindtree.put(v.first, v.second);
        }

        m_scheme.put_child("Scene.shader.bind", bindtree);
    }
     */

    m_scheme.put("Scene.ambient", m_sceneManager->getAmbientLight());
    m_scheme.put("Scene.znear", m_sceneManager->getZNear());
    m_scheme.put("Scene.zfar", m_sceneManager->getZFar());

    for(Iterator<Node*> it = m_rootNode->getChildIterator(); it; it++)
    {
        if(it->isSerialized())
        {
            rtree data = it->serialize(m_filename);
            m_scheme.add_child("Content.node", data);
        }
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

    if(!m_scheme.count("Scene") || !m_scheme.count("Content"))
        throw Exception("SceneParser::load; Invalid scene format (%s)", filepath.c_str());

    m_sceneName = m_scheme.get<string>("Scene");
    m_authorName = m_scheme.get<string>("Scene.author");
    m_attributes = m_scheme.get_child("Scene.attributes", rtree());

    m_filename = filepath;
}

void SceneParser::build()
{
    VectorTranslator<Vector2i> v2itr;
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

    rtree head = m_scheme.get_child("Scene");

    if(head.count("shadow"))
    {
        ShadowMap* smap = m_sceneManager->getShadowMap();
        smap->setEnabled(m_scheme.get<bool>("Scene.shadow", false));
        smap->setFrameSize(m_scheme.get<Vector2i>("Scene.shadow.size", Vector2i(512), v2itr));
        smap->setBlurPass(m_scheme.get<int>("Scene.shadow.blur", 0));
        smap->setIntensity(m_scheme.get<float>("Scene.shadow.intensity", 0.5));
    }

    if(head.count("fog"))
    {
        scene::Fog* fog = m_sceneManager->getFog();
        fog->setEnable(m_scheme.get<bool>("Scene.fog", false));
        fog->setColor(m_scheme.get<Vector4f>("Scene.fog.color", v4ftr));
        fog->setStart(m_scheme.get<float>("Scene.fog.start"));
        fog->setEnd(m_scheme.get<float>("Scene.fog.end"));
    }

    if(head.count("skybox"))
    {
        if(m_scheme.get<bool>("Scene.skybox"))
        {
            TextureTranslator textr;

            Texture skytex[6] = {
                resolve(m_scheme.get<string>("Scene.skybox.front")),
                resolve(m_scheme.get<string>("Scene.skybox.back")),
                resolve(m_scheme.get<string>("Scene.skybox.top")),
                resolve(m_scheme.get<string>("Scene.skybox.bottom")),
                resolve(m_scheme.get<string>("Scene.skybox.left")),
                resolve(m_scheme.get<string>("Scene.skybox.right")),
            };

            scene::SkyBox * sky = m_sceneManager->getSkybox();
            sky->setTextures(skytex);
            sky->setEnable(true);
        }
    }

    if(head.count("shader"))
    {
        Shader shade;

        string path = head.get<string>("shader");

        m_identity.put("Scene.shader", path);

        if(!tools::isAbsoloutPath(path))
            path = resolve(path);

        shade.parseShaderFile(path);

        m_meshScene->setRenderingShader(shade);
    }

    m_sceneManager->setAmbientLight(m_scheme.get<Vector4f>("Scene.ambient", v4ftr));

    m_sceneManager->setZNear(m_scheme.get<float>("Scene.znear"));
    m_sceneManager->setZFar(m_scheme.get<float>("Scene.zfar"));
    m_sceneManager->updateViewParameter();

    BOOST_FOREACH(rtree::value_type &v, m_scheme.get_child("Content"))
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
