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

    void SetRenderFlags(unsigned renderFlags);
    unsigned GetRenderFlags() const;

    bool IsEnable(unsigned flag);
    void Enable(unsigned flag);
    void Disable(unsigned flag);

    void SetTexture(Texture texture, unsigned index = 0);
    Texture GetTexture(unsigned index = 0) const;

    unsigned GetTexturesCount() const;

    void SetShininess(float shininess);
    float GetShininess() const;

    void SetSpecular(Vector4f specular);
    Vector4f GetSpecular() const;

    void SetDiffuse(Vector4f diffuse);
    Vector4f GetDiffuse() const;

    void SetAmbient(Vector4f ambient);
    Vector4f GetAmbient() const;

    void SetShader(const Shader& shader);
    Shader GetShader() const;

    void SetRendWithShader(bool rendWithShader);
    bool IsRendWithShader() const;

    void SetRendWithTexture(bool rendWithTexture);
    bool IsRendWithTexture() const;

    void SetLineWidth(float lineWidth);
    float GetLineWidth() const;

    void SetName(std::string name);
    std::string GetName() const;

    void SetAlphaThershold(float alphaThershold);
    float GetAlphaThershold() const;

    bool IsTransparent();

    void SetFrameSortWait(unsigned frameSortWait);
    unsigned GetFrameSortWait() const;

    void SetTangentLocation(std::string tangentLocation);
    std::string GetTangentLocation() const;

    void SetAoccLocation(std::string aoccLocation);
    std::string GetAoccLocation() const;

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

    void SetFaceType(FaceType faceType);
    FaceType GetFaceType() const;

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
};

}
}

#endif	/* _MATERIAL_H */

