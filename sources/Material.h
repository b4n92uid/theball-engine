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

#include "Clock.h"

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

    enum MulTexBlend
    {
        MODULATE,
        REPLACE,
        ADDITIVE,
    };

    enum
    {
        LIGHTED = 0x1,
        TEXTURED = 0x2,
        COLORED = 0x4,
        FOGED = 0x8,

        SHADER = 0x10,

        TANGENT = 0x20,
        AOCC = 0x40,

        VERTEX_SORT = 0x80,
        VERTEX_SORT_CULL_TRICK = 0x100,

        BLEND_ADD = 0x200,
        BLEND_MUL = 0x400,
        BLEND_MOD = 0x800,

        ALPHA = 0x1000,

        FRONTFACE_CULL = 0x2000,
        BACKFACE_CULL = 0x4000,
    };

    typedef std::vector<Material*> Array;
    typedef std::map<std::string, Material*> Map;

    friend class Mesh;

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

    void setDrawPass(unsigned drawPass);
    unsigned getDrawPass() const;

    /**
     * Spécifier la méthode de mélange utiliser lors du multi-texturing
     */
    void setTextureBlend(MulTexBlend type, unsigned index = 0);

    /**
     * Renvois la méthode de mélange utiliser lors du multi-texturing
     */
    MulTexBlend getTextureBlend(unsigned index = 0);

    void setTextureFrameSize(Vector2i size, unsigned index = 0);
    Vector2i getTextureFrameSize(unsigned index = 0);

    void setTexturePart(Vector2i part, unsigned index = 0);
    Vector2i getTexturePart(unsigned index = 0);

    void setTextureAnimation(unsigned msec, unsigned index = 0);
    unsigned getTextureAnimation(unsigned index = 0);

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
    unsigned m_drawPass;
    float m_lineWidth;
    float m_alphaThershold;

    bool m_depthTest;
    bool m_depthWrite;

    struct TextureApply
    {
        MulTexBlend blend;
        Vector2i frameSize;
        Vector2i part;
        int animation;
        ticks::Clock clock;
    };

    typedef std::map<unsigned, TextureApply> TexApplyMap;

    TexApplyMap m_texApply;
};

}
}

#endif	/* _MATERIAL_H */

