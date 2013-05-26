/* 
 * File:   VolumetricLight.h
 * Author: b4n92uid
 *
 * Created on 21 mai 2013, 21:10
 */

#ifndef VOLUMETRICLIGHT_H
#define	VOLUMETRICLIGHT_H

#include "SceneManager.h"


namespace tbe
{
class Rtt;

namespace scene
{
class Light;

class VolumetricLight
{
public:
    VolumetricLight(Light* light);
    virtual ~VolumetricLight();

    void begin();
    void end();

    void drawLight();
    void render();

    void setFrameSize(Vector2i frameSize);
    Vector2i getFrameSize() const;

    void setLightSize(Vector2f lightSize);
    Vector2f getLightSize() const;

    void setNoiseLayer(Texture noiseLayer);
    Texture getNoiseLayer() const;

    void setOffset(Vector3f offset);
    Vector3f getOffset() const;

private:
    SceneManager* m_scenemng;
    Light* m_light;

    Rtt* m_framebuffer;
    Vector2i m_frameSize;
    Vector2f m_lightSize;
    Vector3f m_offset;

    Shader m_process;
    Texture m_noiseLayer;
};

}
}

#endif	/* VOLUMETRICLIGHT_H */

