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
        "#version 120\n"
        "void main(void)"
        "{"
        "gl_TexCoord[0] = gl_MultiTexCoord0;"
        "gl_Position = ftransform();"
        "}";

static const char fragmentShader[] =
        "#version 120\n"
        "uniform sampler2D tColor;"
        "uniform sampler2D tBlur;"
        "uniform sampler2D tDepth;"
        "void main(void)"
        "{"
        "vec4 vColor = texture2D(tColor, gl_TexCoord[0].st);"
        "vec4 vBlur = texture2D(tBlur, gl_TexCoord[0].st);"

        "float depth = texture2D(tDepth, gl_TexCoord[0].st).r;"

        "float focus = texture2D(tDepth, vec2(0.5)).r;"

        "float n = 3.0;"
        "float f = 256.0;"

        "float depthFactor = (2.0 * n) / (f + n - depth * (f - n));"
        "float focusFactor = (2.0 * n) / (f + n - focus * (f - n));"

        "gl_FragColor = mix(vColor, vBlur, abs(depthFactor - focusFactor) + 0.5);"

        // "gl_FragColor = vec4(abs(depthFactor - focusFactor));"
        "}";

DofEffect::DofEffect()
{
    m_processShader.parseFragmentShader(fragmentShader);
    m_processShader.parseVertexShader(vertexShader);
    m_processShader.loadProgram();

    m_processShader.use(true);
    m_processShader.uniform("tColor", 0);
    m_processShader.uniform("tBlur", 1);
    m_processShader.uniform("tDepth", 2);
    m_processShader.use(false);

    m_blur.setPasse(5);
}

DofEffect::~DofEffect()
{
}

void DofEffect::process(Rtt* rtt)
{
    /*
     Etape du DOF :
     --------------
     Rendue fbo -> workFbo
     Blur sur workFbo
     Rendue workFbo -> fbo (Seulment dans les zones éloigné)
     */

    Texture screenColor = rtt->getColor();
    Texture screenDepth = rtt->getDepht();
    Texture workColor = m_workRtt->getColor();

    // Etape 1 -----------------------------------------------------------------

    m_workRtt->use(true);

    screenColor.use(true);
    m_layer.draw();

    m_workRtt->use(false);

    // Etape 2 -----------------------------------------------------------------

    m_blur.process(m_workRtt);

    // Etape 3 -----------------------------------------------------------------

    rtt->use(true);
    m_processShader.use(true);

    m_layer.begin();

    glActiveTexture(GL_TEXTURE0);
    glClientActiveTexture(GL_TEXTURE0);
    screenColor.use(true);

    glActiveTexture(GL_TEXTURE1);
    glClientActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    workColor.use(true);

    glActiveTexture(GL_TEXTURE2);
    glClientActiveTexture(GL_TEXTURE2);
    glEnable(GL_TEXTURE_2D);
    screenDepth.use(true);

    m_layer.draw(false);

    glActiveTexture(GL_TEXTURE2);
    glClientActiveTexture(GL_TEXTURE2);
    screenColor.use(false);

    glActiveTexture(GL_TEXTURE1);
    glClientActiveTexture(GL_TEXTURE1);
    workColor.use(false);

    glActiveTexture(GL_TEXTURE0);
    glClientActiveTexture(GL_TEXTURE0);
    screenDepth.use(false);

    m_layer.end();

    workColor.use(false);

    m_processShader.use(false);
    rtt->use(false);
}
