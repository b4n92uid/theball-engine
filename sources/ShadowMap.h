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
#include "SceneManager.h"

namespace tbe
{

class Rtt;

namespace scene
{

class Light;
class SceneManager;

class ShadowMapCameraSetup
{
public:
    virtual Vector3f setupCamera(SceneManager*, Light*) = 0;
};

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

    Matrix4 getViewMatrix() const;
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

    void setShaderHandled(bool shaderHandled);
    bool isShaderHandled() const;
    void setCameraSetup(ShadowMapCameraSetup* cameraSetup);
    ShadowMapCameraSetup* getCameraSetup() const;

private:
    Matrix4 m_projectionMatrix;
    Matrix4 m_viewMatrix;
    float m_intensity;

    SceneManager* m_sceneManager;
    ShadowMapCameraSetup* m_cameraSetup;
    ppe::BlurEffect* m_blur;
    Rtt* m_depthBuffer;
    Rtt* m_shadowBuffer;
    Vector2i m_frameSize;
    Shader m_shader;
    bool m_enabled;
    bool m_shaderHandled;
};

}
}

#endif	/* SHADOWMAP_H */

