/* 
 * File:   VolumetricLight.cpp
 * Author: b4n92uid
 * 
 * Created on 21 mai 2013, 21:10
 */

#include "VolumetricLight.h"

#include "Rtt.h"
#include "Light.h"
#include "PostProcessManager.h"

using namespace std;
using namespace tbe;
using namespace scene;

static const char* vertexshader =
        "#version 120\n"
        "void main(void)"
        "{"
        "gl_TexCoord[0] = gl_MultiTexCoord0;"
        "gl_Position = ftransform();"
        "}";

const char* fragmentshader =
        "#version 120\n"

        "uniform sampler2D texture;"
        "uniform sampler2D noise;"
        "uniform vec2 lightpos;"

        "void main(void)"
        "{"
        "const int samples = 200;"
        "const float exposure = 0.0034f;"
        "const float decay = 1.0f;"
        "const float density = 1.00f;"
        "const float weight = 5.00f;"

        "vec2 textCoo = gl_TexCoord[0].st;"
        "vec2 deltaTextCoord = vec2(lightpos.xy - textCoo);"

        "deltaTextCoord *= 1.0 /  float(samples) * density;"

        "float illuminationDecay = 1.0;"

        "for(int i=0; i < samples ; i++)"
        "{"
        "    textCoo += deltaTextCoord;"

        "    vec4 sample = texture2D(texture, textCoo );"
        "    sample *= illuminationDecay * weight;"

        "    gl_FragColor += sample;"

        "    illuminationDecay *= decay;"

        "}"

        "float fadeout = 1.0;"
        "float fadelen = length(lightpos - vec2(0.5));"

        "if(fadelen>0.45)"
        "fadeout = 1 - (fadelen-0.45) / 0.3;"

        "gl_FragColor *= exposure * texture2D(noise, gl_TexCoord[0].st);"
        "gl_FragColor *= fadeout;"
        "}";

bool VolumetricLight::enable = true;

VolumetricLight::VolumetricLight(Light* light)
{
    m_light = light;
    m_scenemng = light->getSceneManager();
    m_frameSize = 512;
    m_lightSize = 64;

    m_framebuffer = new Rtt(m_frameSize);
    m_framebuffer->setCaptureColor(true);
    m_framebuffer->setCaptureDepth(true);

    m_process.parseVertexShader(vertexshader);
    m_process.parseFragmentShader(fragmentshader);
    m_process.loadProgram();

    Shader::bind(m_process);
    m_process.uniform("texture", 0);
    m_process.uniform("noise", 1);
    Shader::unbind();
}

VolumetricLight::~VolumetricLight()
{
    delete m_framebuffer;
}

void VolumetricLight::begin()
{
    m_framebuffer->use(true);

    glClearColor(0, 0, 0, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void VolumetricLight::end()
{
    m_framebuffer->use(false);

    glClearColor(0, 0, 0, 1);
}

void VolumetricLight::drawLight()
{
    Vector3f lp = m_light->getPos() + m_offset;

    if(m_light->getType() == Light::DIRI)
        lp += m_scenemng->getCurCamera()->getPos();

    double view[16];
    double proj[16];

    glGetDoublev(GL_MODELVIEW_MATRIX, view);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    Vector3<double> dscrpos;
    gluProject(lp.x, lp.y, lp.z, view, proj, viewport, &dscrpos.x, &dscrpos.y, &dscrpos.z);

    if(dscrpos.z < 1)
    {
        Vector2f scrpos = math::vec32(dscrpos);

        Shader::bind(m_process);
        m_process.uniform("lightpos", scrpos / m_frameSize);
        Shader::unbind();

        ppe::PostProcessManager::beginPostProcess(m_frameSize.x, m_frameSize.y);

        Vector4f color = m_light->getDiffuse() * 0.5;

        static ppe::Layer lay;
        lay.setGeometry(scrpos - m_lightSize / 2, m_lightSize, color);
        lay.draw();

        ppe::PostProcessManager::endPostProcess();
    }
}

void VolumetricLight::render()
{
    ppe::PostProcessManager::beginPostProcess();
    Shader::bind(m_process);

    static ppe::Layer lay;

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    lay.begin();

    lay.bindTexture(0);
    m_framebuffer->getColor().use(true);

    lay.bindTexture(1);
    m_noiseLayer.use(true);

    lay.draw(false);

    lay.unbindTexture(1);
    lay.unbindTexture(0);
    lay.end();

    Shader::unbind();
    ppe::PostProcessManager::endPostProcess();
}

void VolumetricLight::setFrameSize(Vector2i frameSize)
{
    this->m_frameSize = frameSize;
    m_framebuffer->setFrameSize(m_frameSize);
}

Vector2i VolumetricLight::getFrameSize() const
{
    return m_frameSize;
}

void VolumetricLight::setLightSize(Vector2f lightSize)
{
    this->m_lightSize = lightSize;
}

Vector2f VolumetricLight::getLightSize() const
{
    return m_lightSize;
}

void VolumetricLight::setNoiseLayer(Texture noiseLayer)
{
    this->m_noiseLayer = noiseLayer;
}

Texture VolumetricLight::getNoiseLayer() const
{
    return m_noiseLayer;
}

void VolumetricLight::setOffset(Vector3f offset)
{
    this->m_offset = offset;
}

Vector3f VolumetricLight::getOffset() const
{
    return m_offset;
}
