/* 
 * File:   ShadowMap.h
 * Author: b4n92uid
 *
 * Created on 26 avril 2013, 18:03
 */

#ifndef SHADOWMAP_H
#define	SHADOWMAP_H

#include "Shader.h"
#include "BlurEffect.h"

namespace tbe
{

class Rtt;

namespace scene
{

class Light;
class SceneManager;

class ShadowMap
{
public:
    ShadowMap(SceneManager* sceneManager);
    virtual ~ShadowMap();

    void begin(Light* l);
    void end();

    void bind(Light* l);
    void unbind();

    void render();

    void setFrameSize(Vector2i size);
    Vector2i getFrameSize() const;

    void setEnabled(bool enabled);
    bool isEnabled() const;
    
    void setBlurPass(int blurPass);
    int getBlurPass() const;

private:
    SceneManager* m_sceneManager;
    ppe::BlurEffect* m_blur;
    Rtt* m_lightBuffer;
    Rtt* m_shadowBuffer;
    Vector2i m_frameSize;
    Shader m_shader;
    bool m_enabled;
};

}
}

#endif	/* SHADOWMAP_H */

