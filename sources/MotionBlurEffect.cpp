/*
 * File:   MotionBlurEffect.cpp
 * Author: b4n92uid
 *
 * Created on 5 février 2010, 12:57
 */

#include "MotionBlurEffect.h"

using namespace tbe;
using namespace tbe::ppe;

static const char *vertexShader =
        "void main(void)"
        "{"
        "gl_TexCoord[0] = gl_MultiTexCoord0;"
        "gl_Position = ftransform();"
        "}";


static const char *fragmentShader =
        "uniform sampler2D texture0;"
        "uniform sampler2D texture1;"
        "uniform float intensity;"

        "void main(void)"
        "{"
        "vec4 color0 = texture2D(texture0, gl_TexCoord[0].st);"
        "vec4 color1 = texture2D(texture1, gl_TexCoord[0].st);"
        "gl_FragColor = mix(color0, color1, intensity);"
        "}";

MotionBlurEffect::MotionBlurEffect()
{
    m_processShader.ParseVertexShader(vertexShader);
    m_processShader.ParseFragmentShader(fragmentShader);
    m_processShader.LoadProgram();

    m_processShader.Use(true);
    m_processShader.SetUniform("texture0", 0);
    m_processShader.SetUniform("texture1", 1);
    m_processShader.Use(false);

    SetIntensity(0.9);
}

MotionBlurEffect::~MotionBlurEffect()
{
}

void MotionBlurEffect::Process(Rtt* rtt)
{
    Texture screenColor = rtt->GetColor();
    Texture workColor = m_workRtt->GetColor();

    // Etape 1 -----------------------------------------------------------------

    rtt->Use(true);

    m_layer.Begin();
    m_processShader.Use(true);

    glActiveTexture(GL_TEXTURE0);
    glClientActiveTexture(GL_TEXTURE0);
    screenColor.Use(true);

    glActiveTexture(GL_TEXTURE1);
    glClientActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    workColor.Use(true);

    m_layer.Draw(false);

    glActiveTexture(GL_TEXTURE1);
    glClientActiveTexture(GL_TEXTURE1);
    screenColor.Use(false);

    glActiveTexture(GL_TEXTURE0);
    glClientActiveTexture(GL_TEXTURE0);
    workColor.Use(false);

    m_processShader.Use(false);
    m_layer.End();

    rtt->Use(false);

    // Etape 2 -----------------------------------------------------------------

    m_workRtt->Use(true);

    screenColor.Use(true);

    m_layer.Draw();

    screenColor.Use(false);

    m_workRtt->Use(false);
}

void MotionBlurEffect::SetIntensity(float intensity)
{
    this->m_intensity = intensity;

    m_processShader.Use(true);
    m_processShader.SetUniform("intensity", m_intensity);
    m_processShader.Use(false);
}

float MotionBlurEffect::GetIntensity() const
{
    return m_intensity;
}
