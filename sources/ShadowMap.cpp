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

using namespace tbe;
using namespace scene;

static const char fragment[] =
        "varying vec4 ShadowCoord;"

        "uniform sampler2D ShadowMap;"

        "void main()"
        "{"
        "vec4 shadowCoordinateWdivide = ShadowCoord / ShadowCoord.w ;"

        "shadowCoordinateWdivide.z += 0.0005;"

        "float distanceFromLight = texture2D(ShadowMap,shadowCoordinateWdivide.xy).z;"

        "float shadow = 1.0;"

        "if (ShadowCoord.w > 0.0)"
        "{"
        "if(distanceFromLight > shadowCoordinateWdivide.z)"
        "shadow = 0;"
        "}"

        "gl_FragColor = gl_Color * shadow;"
        "}";

static const char vertex[] =

        "varying vec4 ShadowCoord;"

        "uniform mat4 LightProjectionMatrix;"
        "uniform mat4 LightViewMatrix;"
        "uniform mat4 NodeMatrix;"

        "uniform float Intensity;"

        "void main()"
        "{"
        "mat4 biasMatrix = mat4(0.5, 0.0, 0.0, 0.0,"
        "0.0, 0.5, 0.0, 0.0,"
        "0.0, 0.0, 0.5, 0.0,"
        "0.5, 0.5, 0.5, 1.0);"

        "ShadowCoord = biasMatrix * (LightProjectionMatrix * LightViewMatrix ) * (NodeMatrix * gl_Vertex);"

        "gl_Position = ftransform();"

        "gl_FrontColor = vec4(0,0,0,Intensity);"
        "}";

ShadowMap::ShadowMap(SceneManager* sceneManager)
{
    m_sceneManager = sceneManager;
    m_enabled = false;

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
    m_blur->setOffset(1.0f / 512.0f);
}

ShadowMap::~ShadowMap() { }

void ShadowMap::setEnabled(bool enabled)
{
    this->m_enabled = enabled;
}

bool ShadowMap::isEnabled() const
{
    return m_enabled;
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

Matrix4 ShadowMap::getModelMatrix() const
{
    return m_modelMatrix;
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
    m_blur->setOffset(1.0f / m_frameSize.x);
    m_depthBuffer->setFrameSize(size);
    m_shadowBuffer->setFrameSize(size);
}

Vector2i ShadowMap::getFrameSize() const
{
    return m_frameSize;
}

void ShadowMap::begin(Light* l)
{
    m_depthBuffer->use(true);

    glClear(GL_DEPTH_BUFFER_BIT);

    // Draw back faces into the shadow map
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    // Disable color writes, and use flat shading for speed
    glShadeModel(GL_FLAT);
    glColorMask(0, 0, 0, 0);

    m_projectionMatrix = l->getProjectionMatrix();
    m_modelMatrix = l->getViewMatrix();

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(m_projectionMatrix);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(m_modelMatrix);
}

void ShadowMap::end()
{
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glShadeModel(GL_SMOOTH);
    glColorMask(1, 1, 1, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(m_sceneManager->getProjectionMatrix());

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(m_sceneManager->getViewMatrix());

    m_depthBuffer->use(false);
}

void ShadowMap::bind()
{
    m_shadowBuffer->use(true);

    glClearColor(0, 0, 0, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    Shader::bind(m_shader);

    m_shader.uniform("LightProjectionMatrix", m_projectionMatrix);
    m_shader.uniform("LightViewMatrix", m_modelMatrix);

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