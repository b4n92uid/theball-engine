/* 
 * File:   Water.h
 * Author: b4n92uid
 *
 * Created on 21 mai 2010, 22:02
 */

#ifndef _WATER_H
#define	_WATER_H

#include "Mathematics.h"
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

class WaterParallelScene;

/**
 * \brief Représentation d'un plan d'eau
 */
class Water : public Node
{
public:
    Water(WaterParallelScene* scene);
    Water(const Water& copy);
    ~Water();

    Water& operator=(const Water& copy);

    void render();
    
    void process();
    
    Water* clone();

    void setNormalMap(Texture normalMap);
    Texture getNormalMap() const;

    void setSize(Vector2f size);
    Vector2f getSize() const;

    void beginReflection();
    void endReflection();

    void beginRefraction();
    void endRefraction();

    Rtt& getReflection();
    Rtt& getRefraction();

    void setSpeed(float speed);
    float getSpeed() const;

    void setDeform(float deform);
    float getDeform() const;

    void setBlend(float blend);
    float getBlend() const;

    void setUvRepeat(Vector2f uvRepeat);
    Vector2f getUvRepeat() const;

    rtree serialize(std::string root);

    typedef std::vector<Water*> Array;

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

    WaterParallelScene* m_parallelScene;
};

}
}

#endif	/* _WATER_H */
