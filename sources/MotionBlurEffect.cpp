/*
 * File:   MotionBlurEffect.cpp
 * Author: b4n92uid
 *
 * Created on 5 février 2010, 12:57
 */

#include "MotionBlurEffect.h"
#include "Clock.h"

using namespace tbe;
using namespace tbe::ppe;

static const char *vertexShader =
        "#version 120\n"
        "void main(void)"
        "{"
        "gl_TexCoord[0] = gl_MultiTexCoord0;"
        "gl_Position = ftransform();"
        "}";


static const char *fragmentShader =
        "#version 120\n"
        "uniform sampler2D texture0;"
        "uniform sampler2D texture1;"
        "uniform float intensity;"

        "void main(void)"
        "{"
        "vec4 color0 = texture2D(texture0, gl_TexCoord[0].st);"
        "vec4 color1 = texture2D(texture1, gl_TexCoord[0].st);"
        "gl_FragColor = mix(color0, color1, vec4(intensity));"
        "}";

MotionBlurEffect::MotionBlurEffect()
{
    m_processShader.parseVertexShader(vertexShader);
    m_processShader.parseFragmentShader(fragmentShader);
    m_processShader.loadProgram();

    m_processShader.use(true);
    m_processShader.uniform("texture0", 0);
    m_processShader.uniform("texture1", 1);
    m_processShader.use(false);

    setIntensity(0.9);
}

MotionBlurEffect::~MotionBlurEffect()
{
}

void MotionBlurEffect::process(Rtt* rtt)
{
    Texture screenColor = rtt->getColor();
    Texture workColor = m_workRtt->getColor();

    // Etape 1 -----------------------------------------------------------------

    rtt->use(true);

    m_layer.begin();
    m_processShader.use(true);

    glActiveTexture(GL_TEXTURE0);
    glClientActiveTexture(GL_TEXTURE0);
    screenColor.use(true);

    glActiveTexture(GL_TEXTURE1);
    glClientActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    workColor.use(true);

    m_layer.draw(false);

    glActiveTexture(GL_TEXTURE1);
    glClientActiveTexture(GL_TEXTURE1);
    screenColor.use(false);

    glActiveTexture(GL_TEXTURE0);
    glClientActiveTexture(GL_TEXTURE0);
    workColor.use(false);

    m_processShader.use(false);
    m_layer.end();

    rtt->use(false);

    if(m_clock.isEsplanedTime(33))
    {
        // Etape 2 -------------------------------------------------------------

        m_workRtt->use(true);

        screenColor.use(true);

        m_layer.draw();

        screenColor.use(false);

        m_workRtt->use(false);
    }
}

void MotionBlurEffect::setIntensity(float intensity)
{
    this->m_intensity = intensity;

    m_processShader.use(true);
    m_processShader.uniform("intensity", m_intensity);
    m_processShader.use(false);
}

float MotionBlurEffect::getIntensity() const
{
    return m_intensity;
}
