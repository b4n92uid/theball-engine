/*
 * File:   Light.cpp
 * Author: b4n92uid
 *
 * Created on 14 juillet 2010, 16:11:27
 */

#include "Light.h"

#include "Exception.h"
#include "Tools.h"

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

    Vector3f position = m_matrix.getPos();

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

    ctormap["ambient"] = m_ambient.toStr();
    ctormap["diffuse"] = m_diffuse.toStr();
    ctormap["specular"] = m_specular.toStr();
    ctormap["radius"] = tools::numToStr(m_radius);

    return ctormap;
}

DiriLight::DiriLight(MeshParallelScene* scene) : Light(scene, DIRI)
{

}

PointLight::PointLight(MeshParallelScene* scene) : Light(scene, POINT)
{

}
