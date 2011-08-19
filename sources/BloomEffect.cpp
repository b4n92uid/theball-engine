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
        "uniform bool average;"

        "void main(void)"
        "{"

        "vec4 final = texture2D(texture, gl_TexCoord[0].st);"

        "if(average)"
        "{"
        "   if((final.x + final.y + final.z) / 3.0 > threshold)"
        "      gl_FragColor = final * intensity;"
        "   else "
        "      gl_FragColor = vec4(0, 0, 0, 0);"
        "}"

        "else"
        "{"
        "   if(final.x > threshold || final.y > threshold || final.z > threshold)"
        "      gl_FragColor = final * intensity;"
        "   else "
        "      gl_FragColor = vec4(0, 0, 0, 0);"
        "}"

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

        "if(final == vec4(0))"
        "   discard;"

        "gl_FragColor = final;"

        "}";

BloomEffect::BloomEffect()
{
    m_processShader.parseVertexShader(textureVertexShader);
    m_processShader.parseFragmentShader(brightFragShader);
    m_processShader.loadProgram();

    m_blurShader.parseVertexShader(textureVertexShader);
    m_blurShader.parseFragmentShader(blurFragShader);
    m_blurShader.loadProgram();

    m_blurPass = 1;

    setThreshold(0.5);
    setIntensity(1.0);
}

BloomEffect::~BloomEffect()
{
}

void BloomEffect::process(Rtt* rtt)
{
    /*
     * Etape du bloom :
     * ----------------
     * Rendue bright : fbo -> workFbo (zones �clairer)
     * Blur : workFbo
     * Rendue blend GL_SRC_ALPHA, GL_ONE : workFbo -> output
     */

    // Step 1 ------------------------------------------------------------------

    m_workRtt->use(true);

    m_processShader.use(true);

    rtt->getColor().use(true);
    m_layer.draw();

    m_processShader.use(false);

    // Step 2 ------------------------------------------------------------------

    m_blurShader.use(true);

    m_workRtt->getColor().use(true);

    for(unsigned i = 0; i < m_blurPass; i++)
        m_layer.draw();

    m_blurShader.use(false);

    m_workRtt->use(false);

    // Step 3 ------------------------------------------------------------------

    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    rtt->use(true);

    if(!FrameBufferObject::checkHardware())
    {
        rtt->getColor().use(true);
        m_layer.draw();
    }

    m_workRtt->getColor().use(true);
    m_layer.draw();

    rtt->use(false);

    glPopAttrib();
}

void BloomEffect::setIntensity(float intensity)
{
    this->m_intensity = intensity;

    m_processShader.use(true);
    m_processShader.uniform("intensity", intensity);
    m_processShader.use(false);
}

float BloomEffect::getIntensity() const
{
    return m_intensity;
}

void BloomEffect::setThreshold(float threshold)
{
    this->m_threshold = threshold;

    m_processShader.use(true);
    m_processShader.uniform("threshold", threshold);
    m_processShader.use(false);
}

float BloomEffect::getThreshold() const
{
    return m_threshold;
}

void BloomEffect::setBlurPass(unsigned blurPass)
{
    this->m_blurPass = blurPass;
}

unsigned BloomEffect::getBlurPass() const
{
    return m_blurPass;
}

void BloomEffect::setAverage(bool average)
{
    this->average = average;

    m_processShader.use(true);
    m_processShader.uniform("average", average);
    m_processShader.use(false);
}

bool BloomEffect::isAverage() const
{
    return average;
}
