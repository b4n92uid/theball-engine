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

    void bind();
    void bindMatrix(Matrix4 mat);
    void unbind();

    Matrix4 getModelMatrix() const;
    Matrix4 getProjectionMatrix() const;

    Texture getDepthMap();

    void render();
    
    void renderDebug();

    void setFrameSize(Vector2i size);
    Vector2i getFrameSize() const;

    void setEnabled(bool enabled);
    bool isEnabled() const;

    void setBlurPass(int blurPass);
    int getBlurPass() const;

    void setIntensity(float intensity);
    float getIntensity() const;

private:
    Matrix4 m_projectionMatrix;
    Matrix4 m_modelMatrix;
    float m_intensity;

    SceneManager* m_sceneManager;
    ppe::BlurEffect* m_blur;
    Rtt* m_depthBuffer;
    Rtt* m_shadowBuffer;
    Vector2i m_frameSize;
    Shader m_shader;
    bool m_enabled;
};

}
}

#endif	/* SHADOWMAP_H */

