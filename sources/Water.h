/* 
 * File:   Water.h
 * Author: b4n92uid
 *
 * Created on 21 mai 2010, 22:02
 */

#ifndef _WATER_H
#define	_WATER_H

#include "Mathematics.h"
#include "Matrix.h"
#include "Shader.h"
#include "Texture.h"
#include "HardwareBuffer.h"
#include "Node.h"
#include "Rtt.h"
#include "ParallelScene.h"
#include "Camera.h"
#include "Clock.h"

#include <map>
#include <string>

namespace tbe
{
namespace scene
{

/**
 * \brief Représentation d'un plan d'eau
 */
class Water : public Node
{
public:
    Water();
    Water(const Water& copy);
    ~Water();

    Water& operator=(const Water& copy);

    void Render();
    
    void Process();
    
    Water* Clone();

    void SetNormalMap(Texture normalMap);
    Texture GetNormalMap() const;

    void SetSize(Vector2f size);
    Vector2f GetSize() const;

    void BeginReflection();
    void EndReflection();

    void BeginRefraction();
    void EndRefraction();

    Rtt& GetReflection();
    Rtt& GetRefraction();

    void SetSpeed(float speed);
    float GetSpeed() const;

    void SetDeform(float deform);
    float GetDeform() const;

    void SetBlend(float blend);
    float GetBlend() const;

    void SetUvRepeat(Vector2f uvRepeat);
    Vector2f GetUvRepeat() const;

    typedef std::map<std::string, Water*> Map;

protected:
    HardwareBuffer m_buffer;
    Vector2f m_size;
    Texture m_normalMap;
    Shader m_shader;
    Rtt m_reflection;
    Rtt m_refraction;
    
    ticks::Clock m_clock;

    Vector2f m_uvRepeat;
    float m_blend;
    float m_deform;
    float m_speed;
    float m_uvDecal;
};

}
}

#endif	/* _WATER_H */
