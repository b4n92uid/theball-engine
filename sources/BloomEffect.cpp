/*
 * File:   BloomEffect.cpp
 * Author: b4n92uid
 *
 * Created on 8 janvier 2010, 21:31
 */

#include "BloomEffect.h"

using namespace std;

using namespace tbe;
using namespace tbe::ppe;

static const char textureVertexShader[] =
        "void main(void)"
        "{"
        "gl_TexCoord[0] = gl_MultiTexCoord0;"
        "gl_Position = ftransform();"
        "}";

static const char brightFragShader[] =
        "uniform sampler2D texture;"
        "uniform float threshold;"
        "uniform float intensity;"

        "void main(void)"
        "{"

        "vec4 final = texture2D(texture, gl_TexCoord[0].st);"

        "if(final.x > threshold || final.y > threshold || final.z > threshold)"
        "   gl_FragColor = final * intensity;"

        "else"
        "   gl_FragColor = vec4(0, 0, 0, 0);"

        "}";

static const char blurFragShader[] =
        "uniform sampler2D texture;"

        "void main(void)"
        "{"

        "vec4 final = texture2D(texture, gl_TexCoord[0].st);"

        "float offset = 0.0078125;"

        "final += texture2D(texture, gl_TexCoord[0].st + vec2(offset,0));"
        "final += texture2D(texture, gl_TexCoord[0].st + vec2(-offset,0));"
        "final += texture2D(texture, gl_TexCoord[0].st + vec2(0,offset));"
        "final += texture2D(texture, gl_TexCoord[0].st + vec2(0,-offset));"
        "final += texture2D(texture, gl_TexCoord[0].st + vec2(offset,offset));"
        "final += texture2D(texture, gl_TexCoord[0].st + vec2(offset,-offset));"
        "final += texture2D(texture, gl_TexCoord[0].st + vec2(-offset,-offset));"
        "final += texture2D(texture, gl_TexCoord[0].st + vec2(-offset,offset));"

        "final /= vec4(9);"

        "if(final == 0)"
        "   discard;"

        "gl_FragColor = final;"

        "}";

BloomEffect::BloomEffect()
{
    m_processShader.ParseVertexShader(textureVertexShader);
    m_processShader.ParseFragmentShader(brightFragShader);
    m_processShader.LoadProgram();

    m_blurShader.ParseVertexShader(textureVertexShader);
    m_blurShader.ParseFragmentShader(blurFragShader);
    m_blurShader.LoadProgram();

    m_blurPass = 1;

    SetThreshold(0.5);
    SetIntensity(1.0);
}

BloomEffect::~BloomEffect()
{
}

void BloomEffect::Process(Rtt* rtt)
{
    /*
     * Etape du bloom :
     * ----------------
     * Rendue bright : fbo -> workFbo (zones éclairer)
     * Blur : workFbo
     * Rendue blend GL_SRC_ALPHA, GL_ONE : workFbo -> output
     */

    // Step 1 ------------------------------------------------------------------

    m_workRtt->Use(true);

    m_processShader.Use(true);

    rtt->GetColor().Use(true);
    m_layer.Draw();

    m_processShader.Use(false);

    // Step 2 ------------------------------------------------------------------

    m_blurShader.Use(true);

    m_workRtt->GetColor().Use(true);

    for(unsigned i = 0; i < m_blurPass; i++)
        m_layer.Draw();

    m_blurShader.Use(false);

    m_workRtt->Use(false);

    // Step 3 ------------------------------------------------------------------

    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    rtt->Use(true);

    if(!FrameBufferObject::CheckHardware())
    {
        rtt->GetColor().Use(true);
        m_layer.Draw();
    }

    m_workRtt->GetColor().Use(true);
    m_layer.Draw();

    rtt->Use(false);

    glPopAttrib();
}

void BloomEffect::SetIntensity(float intensity)
{
    this->m_intensity = intensity;

    m_processShader.Use(true);
    m_processShader.SetUniform("intensity", intensity);
    m_processShader.Use(false);
}

float BloomEffect::GetIntensity() const
{
    return m_intensity;
}

void BloomEffect::SetThreshold(float threshold)
{
    this->m_threshold = threshold;

    m_processShader.Use(true);
    m_processShader.SetUniform("threshold", threshold);
    m_processShader.Use(false);
}

float BloomEffect::GetThreshold() const
{
    return m_threshold;
}

void BloomEffect::SetBlurPass(unsigned blurPass)
{
    this->m_blurPass = blurPass;
}

unsigned BloomEffect::GetBlurPass() const
{
    return m_blurPass;
}
