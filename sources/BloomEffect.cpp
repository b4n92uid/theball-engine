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

static const char vertexShader[] =
        "void main(void)"
        "{"
        "gl_TexCoord[0] = gl_MultiTexCoord0;"
        "gl_Position = ftransform();"
        "}";

static const char fragmentShader[] =
        "uniform sampler2D texture;"
        "uniform float threshold;"
        "uniform float intensity;"

        "void main(void)"
        "{"
        "vec4 final = texture2D(texture,gl_TexCoord[0].st);"
        "final *= intensity;"

        "float factor = (final.x + final.y + final.z) / 3.0;"

        "if(factor < threshold)"
        "final = vec4(0, 0, 0, 1);"

        "gl_FragColor = final;"
        "}";

BloomEffect::BloomEffect()
{
    m_useThreshold = true;

    m_processShader.ParseFragmentShader(fragmentShader);
    m_processShader.ParseVertexShader(vertexShader);
    m_processShader.LoadProgram();

    m_blur.SetOffset(1.0f / 128.0f);
    m_blur.SetPasse(5);

    SetThreshold(0.1);
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
     * Rendue fbo -> workFbo (zones éclairer)
     * Blur sur workFbo
     * Rendue de workFbo -> output (blend GL_SRC_ALPHA, GL_ONE)
     */

    // Step 1 ------------------------------------------------------------------

    m_workRtt->Use(true);

    if(m_useThreshold)
    {
        m_processShader.Use(true);

        rtt->GetColor().Use(true);
        m_layer.Draw();

        m_processShader.Use(false);
    }

    else
    {
        glColor4f(1, 1, 1, m_intensity);

        rtt->GetColor().Use(true);
        m_layer.Draw();

        glColor4f(1, 1, 1, 1);
    }

    m_workRtt->Use(false);

    // Step 2 ------------------------------------------------------------------

    m_blur.Process(m_workRtt);

    // Step 4 ------------------------------------------------------------------

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

void BloomEffect::SetBlurPass(unsigned pass)
{
    m_blur.SetPasse(pass);
}

unsigned BloomEffect::GetBlurPass() const
{
    return m_blur.GetPasse();
}

void BloomEffect::SetBlurOffset(float offset)
{
    m_blur.SetOffset(offset);
}

float BloomEffect::GetBlurOffset() const
{
    return m_blur.GetOffset();
}

void BloomEffect::SetUseThreshold(bool useThreshold)
{
    this->m_useThreshold = useThreshold;
}

bool BloomEffect::IsUseThreshold() const
{
    return m_useThreshold;
}
