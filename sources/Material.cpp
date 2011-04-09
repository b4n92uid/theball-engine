/*
 * File:   Material.cpp
 * Author: b4n92uid
 *
 * Created on 16 novembre 2009, 16:56
 */

#include "Material.h"

using namespace tbe;
using namespace tbe::scene;

Material::Material()
{
    m_ambient = 1;
    m_diffuse = 1;
    m_specular = 0;
    m_renderFlags = 0;
    m_shininess = 16;
    m_lineWidth = 0;
    m_alphaThershold = 0;
    m_frameSortWait = 0;
    m_tangentLocation = "tangent";
    m_aoccLocation = "aocc";
    m_faceType = TRIANGLES;
}

Material::Material(const Material& copy)
{
    *this = copy;
}

Material::~Material()
{
}

Material& Material::operator=(const Material& copy)
{
    m_name = copy.m_name;

    m_aoccLocation = copy.m_aoccLocation;
    m_tangentLocation = copy.m_tangentLocation;

    m_ambient = copy.m_ambient;
    m_diffuse = copy.m_diffuse;
    m_specular = copy.m_specular;
    m_shininess = copy.m_shininess;

    m_textures = copy.m_textures;
    m_shader = copy.m_shader;

    m_faceType = copy.m_faceType;

    m_renderFlags = copy.m_renderFlags;

    m_frameSortWait = copy.m_frameSortWait;

    m_lineWidth = copy.m_lineWidth;

    m_alphaThershold = copy.m_alphaThershold;

    return *this;
}

void Material::setAoccLocation(std::string aoccLocation)
{
    this->m_aoccLocation = aoccLocation;
}

std::string Material::getAoccLocation() const
{
    return m_aoccLocation;
}

void Material::setFrameSortWait(unsigned frameSortWait)
{
    this->m_frameSortWait = frameSortWait;
}

unsigned Material::getFrameSortWait() const
{
    return m_frameSortWait;
}

void Material::setTangentLocation(std::string tangentLocation)
{
    this->m_tangentLocation = tangentLocation;
}

std::string Material::getTangentLocation() const
{
    return m_tangentLocation;
}

bool Material::isTransparent()
{
    return (m_renderFlags & BLEND_ADD)
            || (m_renderFlags & BLEND_MUL)
            || (m_renderFlags & BLEND_MOD)
            || (m_renderFlags & ALPHA);
}

void Material::setAlphaThershold(float alphaThershold)
{
    this->m_alphaThershold = alphaThershold;
}

float Material::getAlphaThershold() const
{
    return m_alphaThershold;
}

void Material::setRenderFlags(unsigned renderFlags)
{
    this->m_renderFlags = renderFlags;
}

unsigned Material::getRenderFlags() const
{
    return m_renderFlags;
}

void Material::setShader(const Shader& shader)
{
    m_shader = shader;
}

Shader Material::getShader() const
{
    return m_shader;
}

void Material::setLineWidth(float lineWidth)
{
    this->m_lineWidth = lineWidth;
}

float Material::getLineWidth() const
{
    return m_lineWidth;
}

void Material::setName(std::string name)
{
    this->m_name = name;
}

std::string Material::getName() const
{
    return m_name;
}

void Material::setTexture(Texture texture, unsigned index)
{
    m_textures[index] = texture;
}

Texture Material::getTexture(unsigned index) const
{
    return m_textures.find(index)->second;
}

unsigned Material::getTexturesCount() const
{
    return m_textures.size();
}

void Material::setShininess(float shininess)
{
    m_shininess = shininess;
}

float Material::getShininess() const
{
    return m_shininess;
}

void Material::setSpecular(Vector4f specular)
{
    m_specular = specular;
}

Vector4f Material::getSpecular() const
{
    return m_specular;
}

void Material::setDiffuse(Vector4f diffuse)
{
    m_diffuse = diffuse;
}

Vector4f Material::getDiffuse() const
{
    return m_diffuse;
}

void Material::setAmbient(Vector4f ambient)
{
    m_ambient = ambient;
}

Vector4f Material::getAmbient() const
{
    return m_ambient;
}

bool Material::isEnable(unsigned flag)
{
    return (m_renderFlags & flag);
}

void Material::enable(unsigned flag)
{
    m_renderFlags |= flag;
}

void Material::disable(unsigned flag)
{
    m_renderFlags &= ~flag;
}

void Material::setFaceType(FaceType faceType)
{
    this->m_faceType = faceType;
}

Material::FaceType Material::getFaceType() const
{
    return m_faceType;
}
