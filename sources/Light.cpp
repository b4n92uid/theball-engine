/*
 * File:   Light.cpp
 * Author: b4n92uid
 *
 * Created on 14 juillet 2010, 16:11:27
 */

#include "Light.h"

#include "Exception.h"
#include "Tools.h"
#include "SceneManager.h"
#include "ShadowMap.h"
#include "VolumetricLight.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

// Light -----------------------------------------------------------------------

Light::Light(MeshParallelScene* scene, Type type)
{
    m_ambient = 0;
    m_diffuse = 1;
    m_specular = 0;

    setPos(1);

    m_spotExponent = 0;
    m_spotCutoff = 0;
    m_spotCosCutoff = 0;

    setRadius(1);

    m_type = type;
    m_castShadow = false;
    m_castRays = false;
    m_shadowMap = NULL;
    m_volumeLight = NULL;

    Node::m_parallelScene = m_parallelScene = scene;
    m_sceneManager = m_parallelScene->getSceneManager();

    m_parallelScene->registerLight(this);
}

Light::Light(const Light& orig) : Node(orig)
{
    copy(orig);

    m_parallelScene->registerLight(this);
}

Light::~Light()
{
    m_parallelScene->unregisterLight(this);
}

Light& Light::operator =(const Light& orig)
{
    Node::operator=(orig);

    copy(orig);

    return *this;
}

void Light::copy(const Light& orig)
{
    m_ambient = orig.m_ambient;
    m_diffuse = orig.m_diffuse;
    m_specular = orig.m_specular;
    m_halfVector = orig.m_halfVector;
    m_spotDirection = orig.m_spotDirection;

    m_spotExponent = orig.m_spotExponent;
    m_spotCutoff = orig.m_spotCutoff;
    m_spotCosCutoff = orig.m_spotCosCutoff;

    m_constantAttenuation = orig.m_constantAttenuation;
    m_linearAttenuation = orig.m_linearAttenuation;
    m_quadraticAttenuation = orig.m_quadraticAttenuation;

    m_radius = orig.m_radius;

    m_type = orig.m_type;

    Node::m_parallelScene = m_parallelScene = orig.m_parallelScene;

    m_sceneManager = m_parallelScene->getSceneManager();
}

Light* Light::clone()
{
    return new Light(*this);
}

void Light::process()
{
    if(!m_enable)
        return;

    for_each(m_childs.begin(), m_childs.end(), mem_fun(&Node::process));
}

void Light::render()
{
    int lid = GL_LIGHT0;

    // set ambient color
    glLightfv(lid, GL_AMBIENT, m_ambient);

    // set diffuse color
    glLightfv(lid, GL_DIFFUSE, m_diffuse);

    // set specular color
    glLightfv(lid, GL_SPECULAR, m_specular);

    Vector3f position = getAbsoluteMatrix().getPos();

    switch(m_type)
    {
        case POINT:
            // set attenuation
            // 1.0f / (constant + linear * d + quadratic*(d*d);
            glLightf(lid, GL_CONSTANT_ATTENUATION, m_constantAttenuation);
            glLightf(lid, GL_LINEAR_ATTENUATION, m_linearAttenuation);
            glLightf(lid, GL_QUADRATIC_ATTENUATION, m_quadraticAttenuation);

            // set position
            glLightfv(lid, GL_POSITION, Vector4f(position.x, position.y, position.z, 1.0f));
            break;

        case DIRI:
            // set position
            glLightfv(lid, GL_POSITION, Vector4f(position.x, position.y, position.z, 0));
            break;
    }
}

void Light::setSpecular(Vector4f specular)
{
    this->m_specular = specular;
}

Vector4f Light::getSpecular() const
{
    return m_specular;
}

void Light::setDiffuse(Vector4f diffuse)
{
    this->m_diffuse = diffuse;
}

Vector4f Light::getDiffuse() const
{
    return m_diffuse;
}

void Light::setAmbient(Vector4f ambient)
{
    this->m_ambient = ambient;
}

Vector4f Light::getAmbient() const
{
    return m_ambient;
}

void Light::setRadius(float value)
{
    m_radius = value;
    m_constantAttenuation = 0;
    m_quadraticAttenuation = 0;
    m_linearAttenuation = 1.0f / value;
}

float Light::getRadius() const
{
    return m_radius;
}

void Light::setType(Type type)
{
    this->m_type = type;
}

Light::Type Light::getType() const
{
    return m_type;
}

void Light::setCastShadow(bool castShadow)
{
    this->m_castShadow = castShadow;

    if(!m_shadowMap)
        m_shadowMap = new ShadowMap(this);
}

bool Light::isCastShadow() const
{
    return m_castShadow;
}

void Light::setCastRays(bool castRays)
{
    this->m_castRays = castRays;

    if(!m_volumeLight)
        m_volumeLight = new VolumetricLight(this);
}

bool Light::isCastRays() const
{
    return m_castRays;
}

rtree Light::serialize(std::string root)
{
    rtree scheme = Node::serialize(root);

    scheme.put("class", "Light");

    if(m_type == Light::DIRI)
        scheme.put("class.type", "diri");
    else if(m_type == Light::POINT)
        scheme.put("class.type", "point");

    scheme.put("class.ambient", m_ambient.toStr());
    scheme.put("class.diffuse", m_diffuse.toStr());
    scheme.put("class.specular", m_specular.toStr());
    scheme.put("class.radius", m_radius);

    scheme.put("class.castShadow", m_castShadow);
    scheme.put("class.castRays", m_castRays);

    if(m_shadowMap)
    {
        ShadowMap* smap = m_shadowMap;
        scheme.put("class.shadowMap.size", smap->getFrameSize().toStr());
        scheme.put("class.shadowMap.blur", smap->getBlurPass());
        scheme.put("class.shadowMap.intensity", smap->getIntensity());
        scheme.put("class.shadowMap.shader", smap->isShaderHandled());
    }

    if(m_volumeLight)
    {
        VolumetricLight* vlight = m_volumeLight;
        scheme.put("class.volumeLight.frameSize", vlight->getFrameSize().toStr());
        scheme.put("class.volumeLight.lightSize", vlight->getLightSize().toStr());
        scheme.put("class.volumeLight.offset", vlight->getOffset().toStr());

        if(vlight->getNoiseLayer())
        {
            string path = tools::relativizePath(vlight->getNoiseLayer().getFilename(), root);
            scheme.put("class.volumeLight.noiseLayer", path);
        }
    }

    return scheme;
}

MeshParallelScene* Light::getParallelScene() const
{
    return m_parallelScene;
}

void Light::setShadowMap(ShadowMap* shadowMap)
{
    if(m_shadowMap)
        delete m_shadowMap;

    this->m_shadowMap = shadowMap;
}

ShadowMap* Light::getShadowMap() const
{
    return m_shadowMap;
}

void Light::setVolumeLight(VolumetricLight* volumeLight)
{
    this->m_volumeLight = volumeLight;
}

VolumetricLight* Light::getVolumeLight() const
{
    return m_volumeLight;
}

DiriLight::DiriLight(MeshParallelScene* scene) : Light(scene, DIRI) { }

PointLight::PointLight(MeshParallelScene* scene) : Light(scene, POINT) { }
