#include "Light.h"
#include "Exception.h"


using namespace std;
using namespace tbe;
using namespace tbe::scene;

// Light -----------------------------------------------------------------------

Light::Light(Type type)
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

    SetPos(1);

    m_spotExponent = 0;
    m_spotCutoff = 0;
    m_spotCosCutoff = 0;

    SetRadius(1);

    m_type = type;
}

Light::Light(const Light& copy)
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
}

Light::~Light()
{
    if(m_lightId != -1)
        glDisable(m_lightId);
}

Light& Light::operator =(const Light& copy)
{
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

    m_lightId = copy.m_lightId;

    m_type = copy.m_type;

    return *this;
}

Light* Light::Clone()
{
    return new Light(*this);
}

void Light::Process()
{

}

void Light::Render()
{
    m_enable ? glEnable(m_lightId) : glDisable(m_lightId);

    if(!m_enable || !m_enableRender)
        return;

    // set ambient color
    glLightfv(m_lightId, GL_AMBIENT, m_ambient);

    // set diffuse color
    glLightfv(m_lightId, GL_DIFFUSE, m_diffuse);

    // set specular color
    glLightfv(m_lightId, GL_SPECULAR, m_specular);

    Vector3f position = GetPos();

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

void Light::SetSpecular(Vector4f specular)
{
    this->m_specular = specular;
}

Vector4f Light::GetSpecular() const
{
    return m_specular;
}

void Light::SetDiffuse(Vector4f diffuse)
{
    this->m_diffuse = diffuse;
}

Vector4f Light::GetDiffuse() const
{
    return m_diffuse;
}

void Light::SetAmbient(Vector4f ambient)
{
    this->m_ambient = ambient;
}

Vector4f Light::GetAmbient() const
{
    return m_ambient;
}

void Light::SetRadius(float value)
{
    m_radius = value;
    m_constantAttenuation = 0;
    m_quadraticAttenuation = 0;
    m_linearAttenuation = 1.0f / value;
}

float Light::GetRadius() const
{
    return m_radius;
}

void Light::SetType(Type type)
{
    this->m_type = type;
}

Light::Type Light::GetType() const
{
    return m_type;
}

DiriLight::DiriLight() : Light(DIRI)
{

}

PointLight::PointLight() : Light(POINT)
{

}
