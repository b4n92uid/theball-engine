/*
 * File:   Light.cpp
 * Author: b4n92uid
 *
 * Created on 14 juillet 2010, 16:11:27
 */

#include "Light.h"
#include "Exception.h"
#include "LightParallelScene.h"
#include "Tools.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

// Light -----------------------------------------------------------------------

Light::Light(LightParallelScene* scene, Type type)
{
    GLint maxLight;
    glGetIntegerv(GL_MAX_LIGHTS, & maxLight);

    for(m_lightId = GL_LIGHT0; true; m_lightId++)
    {
        if(!glIsEnabled(m_lightId))
            break;

        if(m_lightId >= GL_LIGHT0 + maxLight)
            throw Exception("Light::Light; Add light error; Max light allowed %d", maxLight);
    }

    glEnable(m_lightId);

    m_ambient = 0;
    m_diffuse = 1;
    m_specular = 0;

    setPos(1);

    m_spotExponent = 0;
    m_spotCutoff = 0;
    m_spotCosCutoff = 0;

    setRadius(1);

    m_type = type;

    Node::m_parallelScene = m_parallelScene = scene;
    m_sceneManager = m_parallelScene->getSceneManager();

    m_parallelScene->registerNode(this);
}

Light::Light(const Light& copy) : Node(copy)
{
    GLint maxLight;
    glGetIntegerv(GL_MAX_LIGHTS, & maxLight);

    for(m_lightId = GL_LIGHT0; true; m_lightId++)
    {
        if(!glIsEnabled(m_lightId))
            break;

        if(m_lightId >= GL_LIGHT0 + maxLight)
            throw Exception("Light::Light; Add light error; Max light allowed %d", maxLight);
    }

    glEnable(m_lightId);

    *this = copy;

    m_parallelScene->registerNode(this);
}

Light::~Light()
{
    if(m_lightId != -1)
        glDisable(m_lightId);

    m_parallelScene->unregisterNode(this);
}

Light& Light::operator =(const Light& copy)
{
    Node::operator=(copy);

    m_ambient = copy.m_ambient;
    m_diffuse = copy.m_diffuse;
    m_specular = copy.m_specular;
    m_halfVector = copy.m_halfVector;
    m_spotDirection = copy.m_spotDirection;

    m_spotExponent = copy.m_spotExponent;
    m_spotCutoff = copy.m_spotCutoff;
    m_spotCosCutoff = copy.m_spotCosCutoff;

    m_constantAttenuation = copy.m_constantAttenuation;
    m_linearAttenuation = copy.m_linearAttenuation;
    m_quadraticAttenuation = copy.m_quadraticAttenuation;

    m_radius = copy.m_radius;

    m_type = copy.m_type;

    Node::m_parallelScene = m_parallelScene = copy.m_parallelScene;

    m_sceneManager = m_parallelScene->getSceneManager();

    return *this;
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
    m_enable ? glEnable(m_lightId) : glDisable(m_lightId);

    if(!m_enable)
        return;

    // set ambient color
    glLightfv(m_lightId, GL_AMBIENT, m_ambient);

    // set diffuse color
    glLightfv(m_lightId, GL_DIFFUSE, m_diffuse);

    // set specular color
    glLightfv(m_lightId, GL_SPECULAR, m_specular);

    Vector3f position = getPos();

    switch(m_type)
    {
        case POINT:
            // set attenuation
            // 1.0f / (constant + linear * d + quadratic*(d*d);
            glLightf(m_lightId, GL_CONSTANT_ATTENUATION, m_constantAttenuation);
            glLightf(m_lightId, GL_LINEAR_ATTENUATION, m_linearAttenuation);
            glLightf(m_lightId, GL_QUADRATIC_ATTENUATION, m_quadraticAttenuation);

            // set position
            glLightfv(m_lightId, GL_POSITION, Vector4f(position.x, position.y, position.z, 1.0f));
            break;

        case DIRI:
            // set position
            glLightfv(m_lightId, GL_POSITION, Vector4f(position.x, position.y, position.z, 0));
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

Node::CtorMap Light::constructionMap(std::string root)
{
    Node::CtorMap ctormap = Node::constructionMap(root);

    ctormap["class"] = "Light";

    if(m_type == Light::DIRI)
        ctormap["type"] = "Diri";
    else if(m_type == Light::POINT)
        ctormap["type"] = "Point";
    else
        ctormap["type"] = tools::numToStr(m_type);

    ctormap["ambient"] = tools::vec4ToStr(m_ambient);
    ctormap["diffuse"] = tools::vec4ToStr(m_diffuse);
    ctormap["specular"] = tools::vec4ToStr(m_specular);

    return ctormap;
}

DiriLight::DiriLight(LightParallelScene* scene) : Light(scene, DIRI)
{

}

PointLight::PointLight(LightParallelScene* scene) : Light(scene, POINT)
{

}
