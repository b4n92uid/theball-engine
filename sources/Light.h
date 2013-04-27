#ifndef LIGHT_H
#define LIGHT_H

#include <iostream>
#include <map>

#include "GLee.h"

#include "Mathematics.h"
#include "Node.h"
#include "MeshParallelScene.h"

namespace tbe
{
namespace scene
{

class LightingTechnique;

/**
 * \brief Représentation d'un lumieres
 */
class Light : public Node
{
public:

    enum Type
    {
        POINT,
        DIRI,
    };

    Light(MeshParallelScene* scene, Type type = POINT);
    Light(const Light& orig);
    ~Light();

    Light & operator=(const Light& orig);

    void process();

    void render();

    Light* clone();

    void setSpecular(Vector4f specular);
    Vector4f getSpecular() const;

    void setDiffuse(Vector4f diffuse);
    Vector4f getDiffuse() const;

    void setAmbient(Vector4f ambient);
    Vector4f getAmbient() const;

    void setRadius(float value);
    float getRadius() const;

    void setType(Type type);
    Type getType() const;

    Matrix4 getProjectionMatrix() const;
    Matrix4 getViewMatrix() const;

    void setCastShadow(bool castShadow);
    bool isCastShadow() const;

    rtree serialize(std::string root);

    typedef std::vector<scene::Light*> Array;

protected:
    Vector4f m_ambient; // Acli
    Vector4f m_diffuse; // Dcli
    Vector4f m_specular; // Scli
    Vector3f m_halfVector; // Derived: Hi
    Vector3f m_spotDirection; // Sdli

    bool m_castShadow;

    float m_spotExponent; // Srli
    float m_spotCutoff; // Crli
    float m_spotCosCutoff; // Derived: cos(Crli) (range: [0.0,90.0], 180.0)

    float m_constantAttenuation; // K0 (range: [1.0,0.0],-1.0)
    float m_linearAttenuation; // K1
    float m_quadraticAttenuation; // K2

    float m_radius;

    Type m_type;

    MeshParallelScene* m_parallelScene;

private:
    void copy(const Light& orig);
};

/// \brief Lumiere dirictionelle

class DiriLight : public Light
{
public:
    DiriLight(MeshParallelScene* scene);
};

/// \brief Lumiere pointctuelle

class PointLight : public Light
{
public:
    PointLight(MeshParallelScene* scene);
};

}
}

#endif // LIGHT_H
