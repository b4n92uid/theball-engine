/* 
 * File:   DofEffect.cpp
 * Author: b4n92uid
 * 
 * Created on 10 janvier 2010, 20:23
 */

#include "DofEffect.h"

using namespace std;

using namespace tbe;
using namespace tbe::ppe;

static const char vertexShader[] =
        "void main(void)"
        "{"
        "gl_TexCoord[0] = gl_MultiTexCoord0;"
        "gl_Position = ftransform();"
        "}";

static const char fragmentShader[] =
        "uniform sampler2D tColor;"
        "uniform sampler2D tBlur;"
        "uniform sampler2D tDepth;"
        "void main(void)"
        "{"
        "vec4 vColor = texture2D(tColor, gl_TexCoord[0].st);"
        "vec4 vBlur = texture2D(tBlur, gl_TexCoord[0].st);"

        "float depth = texture2D(tDepth, gl_TexCoord[0].st).r;"

        "float n = 1.0;"
        "float f = 100.0;"
        "float z = depth;"

        "float factor = (2.0 * n) / (f + n - z * (f - n));"

        "gl_FragColor = mix(vColor, vBlur, factor);"
        "}";

DofEffect::DofEffect()
{
    m_processShader.ParseFragmentShader(fragmentShader);
    m_processShader.ParseVertexShader(vertexShader);
    m_processShader.LoadProgram();

    m_processShader.Use(true);
    m_processShader.SetUniform("tColor", 0);
    m_processShader.SetUniform("tBlur", 1);
    m_processShader.SetUniform("tDepth", 2);
    m_processShader.Use(false);

    m_blur.SetOffset(1.0f / 128.0f);
    m_blur.SetPasse(10);
}

DofEffect::~DofEffect()
{
}

void DofEffect::Process(Rtt* rtt)
{
    /*
     Etape du DOF :
     --------------
     Rendue fbo -> workFbo
     Blur sur workFbo
     Rendue workFbo -> fbo (Seulment dans les zones éloigné)
     */

    Texture screenColor = rtt->GetColor();
    Texture screenDepth = rtt->GetDepht();
    Texture workColor = m_workRtt->GetColor();

    // Etape 1 -----------------------------------------------------------------

    m_workRtt->Use(true);

    screenColor.Use(true);
    m_layer.Draw();

    m_workRtt->Use(false);

    // Etape 2 -----------------------------------------------------------------

    m_blur.Process(m_workRtt);

    // Etape 3 -----------------------------------------------------------------

    rtt->Use(true);
    m_processShader.Use(true);

    m_layer.Begin();

    glActiveTexture(GL_TEXTURE0);
    glClientActiveTexture(GL_TEXTURE0);
    screenColor.Use(true);

    glActiveTexture(GL_TEXTURE1);
    glClientActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    workColor.Use(true);

    glActiveTexture(GL_TEXTURE2);
    glClientActiveTexture(GL_TEXTURE2);
    glEnable(GL_TEXTURE_2D);
    screenDepth.Use(true);

    m_layer.Draw(false);

    glActiveTexture(GL_TEXTURE2);
    glClientActiveTexture(GL_TEXTURE2);
    screenColor.Use(false);

    glActiveTexture(GL_TEXTURE1);
    glClientActiveTexture(GL_TEXTURE1);
    workColor.Use(false);

    glActiveTexture(GL_TEXTURE0);
    glClientActiveTexture(GL_TEXTURE0);
    screenDepth.Use(false);

    m_layer.End();

    workColor.Use(false);

    m_processShader.Use(false);
    rtt->Use(false);
}
