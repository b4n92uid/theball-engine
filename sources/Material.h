/*
 * File:   Material.h
 * Author: b4n92uid
 *
 * Created on 16 novembre 2009, 16:56
 */

#ifndef _MATERIAL_H
#define	_MATERIAL_H

#include <map>
#include <string>

#include "Mathematics.h"
#include "Texture.h"
#include "Shader.h"

namespace tbe
{
namespace scene
{

/**
 * \brief Représentation d'un matériau
 * Stock les propriétés de rendue d'un maillage
 */
class Material
{
public:

    Material();
    Material(const Material& copy);
    ~Material();

    Material & operator=(const Material& copy);

    void setRenderFlags(unsigned renderFlags);
    unsigned getRenderFlags() const;

    bool isEnable(unsigned flag);
    void enable(unsigned flag);
    void disable(unsigned flag);

    void setTexture(Texture texture, unsigned index = 0);
    Texture getTexture(unsigned index = 0) const;
    void dropTexture(unsigned index = 0);

    unsigned getTexturesCount() const;

    void setShininess(float shininess);
    float getShininess() const;

    void setSpecular(Vector4f specular);
    Vector4f getSpecular() const;

    void setDiffuse(Vector4f diffuse);
    Vector4f getDiffuse() const;

    void setAmbient(Vector4f ambient);
    Vector4f getAmbient() const;

    void setShader(const Shader& shader);
    Shader getShader() const;

    void setRendWithShader(bool rendWithShader);
    bool isRendWithShader() const;

    void setRendWithTexture(bool rendWithTexture);
    bool isRendWithTexture() const;

    void setLineWidth(float lineWidth);
    float getLineWidth() const;

    void setName(std::string name);
    std::string getName() const;

    void setAlphaThershold(float alphaThershold);
    float getAlphaThershold() const;

    bool isTransparent();

    void setFrameSortWait(unsigned frameSortWait);
    unsigned getFrameSortWait() const;

    void setTangentLocation(std::string tangentLocation);
    std::string getTangentLocation() const;

    void setAoccLocation(std::string aoccLocation);
    std::string getAoccLocation() const;

    enum FaceType
    {
        QUADS = GL_QUADS,
        TRIANGLES = GL_TRIANGLES,
        TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
        POLYGON = GL_POLYGON,
        LINES = GL_LINES,
        LINE_STRIP = GL_LINE_STRIP,
        LINE_LOOP = GL_LINE_LOOP,
    };

    void setFaceType(FaceType faceType);
    FaceType getFaceType() const;

    void setDepthWrite(bool depthWrite);
    bool isDepthWrite() const;

    void setDepthTest(bool depthTest);
    bool isDepthTest() const;

    enum
    {
        LIGHT = 0x1,
        TEXTURE = 0x2,
        COLOR = 0x4,
        SHADER = 0x8,

        TANGENT = 0x10,
        AOCC = 0x2000,

        VERTEX_SORT = 0x20,
        VERTEX_SORT_CULL_TRICK = 0x40,

        BLEND_ADD = 0x80,
        BLEND_MUL = 0x100,
        BLEND_MOD = 0x200,

        ALPHA = 0x400,

        FRONTFACE_CULL = 0x800,
        BACKFACE_CULL = 0x1000,
    };

    typedef std::vector<Material*> Array;
    typedef std::map<std::string, Material*> Map;

    friend class Mesh;

protected:
    std::string m_name;

    std::string m_aoccLocation;
    std::string m_tangentLocation;

    Vector4f m_ambient, m_diffuse, m_specular;
    float m_shininess;

    Texture::Map m_textures;

    Shader m_shader;

    FaceType m_faceType;

    unsigned m_renderFlags;

    unsigned m_frameSortWait;
    float m_lineWidth;
    float m_alphaThershold;

    bool m_depthTest;
    bool m_depthWrite;
};

}
}

#endif	/* _MATERIAL_H */

