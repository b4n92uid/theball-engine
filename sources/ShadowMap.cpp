/* 
 * File:   ShadowMap.cpp
 * Author: b4n92uid
 * 
 * Created on 26 avril 2013, 18:03
 */

#include "GLee.h"

#include "ShadowMap.h"
#include "SceneManager.h"
#include "Rtt.h"
#include "Light.h"
#include "PostProcessManager.h"
#include "BlurEffect.h"

using namespace std;
using namespace tbe;
using namespace scene;

static const char fragment[] =
        "varying vec4 ShadowCoord;"
        "uniform sampler2D ShadowMap;"
        "uniform float Intensity;"

        "void main()"
        "{"
        "vec4 wdiv = ShadowCoord / ShadowCoord.w ;"
        "wdiv.z -= 0.0005;"

        "float offset = 0.0009765625;"
        "float distanceFromLight = texture2D(ShadowMap,wdiv.st).z;"
        "float shadow = 1.0;"

        "if(wdiv.x < 0 || wdiv.y < 0) shadow = 1.0;"

        "else if(wdiv.x > 1 || wdiv.y > 1) shadow = 1.0;"

        "else if (ShadowCoord.w > 0.0 && distanceFromLight < wdiv.z)"
        "{"
        "shadow = texture2D(ShadowMap, wdiv.xy).r;"
        "shadow += texture2D(ShadowMap, wdiv.xy + vec2(-offset, offset)).r;"
        "shadow += texture2D(ShadowMap, wdiv.xy + vec2(0,       offset)).r;"
        "shadow += texture2D(ShadowMap, wdiv.xy + vec2(offset,  offset)).r;"
        "shadow += texture2D(ShadowMap, wdiv.xy + vec2(offset,  0)).r;"
        "shadow += texture2D(ShadowMap, wdiv.xy + vec2(-offset, 0)).r;"
        "shadow += texture2D(ShadowMap, wdiv.xy + vec2(-offset, -offset)).r;"
        "shadow += texture2D(ShadowMap, wdiv.xy + vec2(0,       -offset)).r;"
        "shadow += texture2D(ShadowMap, wdiv.xy + vec2(offset,  -offset)).r;"
        "shadow /= 9;"
        "}"

        "gl_FragColor = vec4(0,0,0,Intensity+0.5) * (1-shadow);"
        "}";

static const char vertex[] =
        "varying vec4 ShadowCoord;"

        "uniform mat4 LightProjectionMatrix;"
        "uniform mat4 LightViewMatrix;"
        "uniform mat4 NodeMatrix;"

        "void main()"
        "{"
        "mat4 biasMatrix = mat4(0.5, 0.0, 0.0, 0.0,"
        "0.0, 0.5, 0.0, 0.0,"
        "0.0, 0.0, 0.5, 0.0,"
        "0.5, 0.5, 0.5, 1.0);"

        "ShadowCoord = biasMatrix * (LightProjectionMatrix * LightViewMatrix ) * (NodeMatrix * gl_Vertex);"

        "gl_Position = ftransform();"
        "}";

ShadowMap::ShadowMap(Light* light)
{
    m_light = light;
    m_sceneManager = light->getSceneManager();
    m_shaderHandled = false;
    m_cameraSetup = NULL;

    m_shader.parseVertexShader(vertex);
    m_shader.parseFragmentShader(fragment);
    m_shader.loadProgram();

    setIntensity(0.5);

    m_frameSize = 512;

    m_depthBuffer = new Rtt(m_frameSize);
    m_depthBuffer->setCaptureColor(true);
    m_depthBuffer->setCaptureDepth(true);

    m_shadowBuffer = new Rtt(m_frameSize);
    m_shadowBuffer->setCaptureColor(true);
    m_shadowBuffer->setCaptureDepth(true);

    m_blur = new ppe::BlurEffect;
    m_blur->setRttFrameSize(m_frameSize);
    m_blur->setPasse(2);
}

ShadowMap::~ShadowMap()
{
    delete m_depthBuffer;
    delete m_shadowBuffer;
    delete m_blur;

    if(m_cameraSetup)
        delete m_cameraSetup;
}

void ShadowMap::setBlurPass(int blurPass)
{
    m_blur->setPasse(blurPass);
}

int ShadowMap::getBlurPass() const
{
    return m_blur->getPasse();
}

void ShadowMap::setIntensity(float intensity)
{
    this->m_intensity = intensity;

    Shader::bind(m_shader);
    m_shader.uniform("Intensity", intensity);
    Shader::unbind();
}

float ShadowMap::getIntensity() const
{
    return m_intensity;
}

void ShadowMap::setShaderHandled(bool shaderHandled)
{
    this->m_shaderHandled = shaderHandled;
}

bool ShadowMap::isShaderHandled() const
{
    return m_shaderHandled;
}

void ShadowMap::setCameraSetup(ShadowMapCameraSetup* cameraSetup)
{
    this->m_cameraSetup = cameraSetup;
}

ShadowMapCameraSetup* ShadowMap::getCameraSetup() const
{
    return m_cameraSetup;
}

Matrix4 ShadowMap::getViewMatrix() const
{
    return m_viewMatrix;
}

Matrix4 ShadowMap::getProjectionMatrix() const
{
    return m_projectionMatrix;
}

Texture ShadowMap::getDepthMap()
{
    return m_depthBuffer->getDepht();
}

void ShadowMap::setFrameSize(Vector2i size)
{
    m_frameSize = size;
    m_blur->setRttFrameSize(m_frameSize);
    m_depthBuffer->setFrameSize(size);
    m_shadowBuffer->setFrameSize(size);
}

Vector2i ShadowMap::getFrameSize() const
{
    return m_frameSize;
}

void ShadowMap::begin()
{
    m_depthBuffer->use(true);

    glClear(GL_DEPTH_BUFFER_BIT);

    // Disable color writes, and use flat shading for speed
    glShadeModel(GL_FLAT);
    glColorMask(0, 0, 0, 1);

    Camera* cam = m_sceneManager->getCurCamera();

    float length = m_light->getParallelScene()->getSceneAabb().getLength() / 4.0f;
    length /= 3;

    Vector3f centerView = cam->getPos() + cam->getTarget() * length;

    if(m_cameraSetup)
        centerView = m_cameraSetup->setupCamera(m_sceneManager, m_light);

    Vector3f pos = centerView + m_light->getPos().normalize();
    Vector3f target = centerView;

    if(m_cameraSetup)
        m_projectionMatrix = m_cameraSetup->setupMatrix(m_sceneManager, m_light);
    else
        m_projectionMatrix = math::orthographicMatrix(-length * 2, length * 2, -length, length, -length, length);

    m_viewMatrix = math::lookAt(pos, target, Vector3f(0.0f, 1.0f, 0.0f));

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(m_projectionMatrix);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(m_viewMatrix);

    m_sceneManager->getFrustum()->extractPlane();
}

void ShadowMap::end()
{
    glShadeModel(GL_SMOOTH);
    glColorMask(1, 1, 1, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(m_sceneManager->getProjectionMatrix());

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(m_sceneManager->getViewMatrix());

    m_sceneManager->getFrustum()->extractPlane();

    m_depthBuffer->use(false);
}

void ShadowMap::bind()
{
    m_shadowBuffer->use(true);

    glClearColor(0, 0, 0, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);

    Shader::bind(m_shader);

    m_shader.uniform("LightProjectionMatrix", m_projectionMatrix);
    m_shader.uniform("LightViewMatrix", m_viewMatrix);

    m_depthBuffer->getDepht().use(true);
}

void ShadowMap::bindMatrix(Matrix4 mat)
{
    m_shader.uniform("NodeMatrix", mat);
}

void ShadowMap::unbind()
{
    m_depthBuffer->getDepht().use(false);

    Shader::unbind();

    glClearColor(0, 0, 0, 1);

    m_shadowBuffer->use(false);
}

void ShadowMap::render()
{
    ppe::PostProcessManager::beginPostProcess();

    m_blur->process(m_shadowBuffer);

    ppe::Layer lay;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_shadowBuffer->getColor().use(true);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    lay.draw();

    m_shadowBuffer->getColor().use(false);

    ppe::PostProcessManager::endPostProcess();
}

void ShadowMap::renderDebug()
{
    ppe::PostProcessManager::beginPostProcess();

    ppe::Layer lay(0, 0.25);

    m_depthBuffer->getDepht().use(true);

    lay.draw();

    m_depthBuffer->getDepht().use(false);

    ppe::PostProcessManager::endPostProcess();
}