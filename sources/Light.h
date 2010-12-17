#ifndef LIGHT_H
#define LIGHT_H

#include <iostream>
#include <map>

#include "GLee.h"

#include "Mathematics.h"
#include "Node.h"

namespace tbe
{
namespace scene
{

/**
 * \brief Représentation d'un lumieres
 */
class Light : public Node
{
public:

    enum Type
    {
        DIRI,
        POINT,
    };

    Light(Type type = POINT);
    Light(const Light& copy);
    ~Light();

    Light & operator=(const Light& copy);

    void Process();
    void Render();

    Light* Clone();

    void SetSpecular(Vector4f specular);
    Vector4f GetSpecular() const;

    void SetDiffuse(Vector4f diffuse);
    Vector4f GetDiffuse() const;

    void SetAmbient(Vector4f ambient);
    Vector4f GetAmbient() const;

    void SetRadius(float value);
    float GetRadius() const;

    void SetType(Type type);
    Type GetType() const;

    typedef std::vector<scene::Light*> Array;

protected:
    Vector4f m_ambient; // Acli
    Vector4f m_diffuse; // Dcli
    Vector4f m_specular; // Scli
    Vector3f m_halfVector; // Derived: Hi
    Vector3f m_spotDirection; // Sdli

    float m_spotExponent; // Srli
    float m_spotCutoff; // Crli
    float m_spotCosCutoff; // Derived: cos(Crli) (range: [0.0,90.0], 180.0)

    float m_constantAttenuation; // K0 (range: [1.0,0.0],-1.0)
    float m_linearAttenuation; // K1
    float m_quadraticAttenuation; // K2

    float m_radius;

    GLint m_lightId;

    Type m_type;
};

/// \brief Lumiere dirictionelle

class DiriLight : public Light
{
public:
    DiriLight();
};

/// \brief Lumiere pointctuelle

class PointLight : public Light
{
public:
    PointLight();
};

}
}

#endif // LIGHT_H
