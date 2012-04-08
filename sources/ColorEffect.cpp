/* 
 * File:   ColorEffect.cpp
 * Author: b4n92uid
 * 
 * Created on 22 janvier 2010, 18:54
 */

#include "ColorEffect.h"

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
        "uniform sampler2D texture;"
        "uniform vec4 color;"
        "uniform int fusionMode;"

        "void main(void)"
        "{"

        "vec4 final = texture2D(texture, gl_TexCoord[0].st);"

        "if(fusionMode == 0)"
        "final += color;"

        "else if(fusionMode == 1)"
        "final -= color;"

        "else if(fusionMode == 2)"
        "final *= color;"

        "else if(fusionMode == 3)"
        "final /= color;"

        "else if(fusionMode == 4)"
        "final = (color + final) / vec4(2.0);"

        "else if(fusionMode == 5)"
        "{"
        "vec4 set = final * color;"
        "final = (set.r + set.g + set.b) / vec4(3.0);"
        "}"

        "else if(fusionMode == 6)"
        "final = (1.0f - final);"

        "final.a = color.a;"

        "gl_FragColor = final;"
        "}";

ColorEffect::ColorEffect()
{
    if(Shader::checkHardware())
    {
        m_processShader.parseFragmentShader(fragmentShader);
        m_processShader.parseVertexShader(vertexShader);
        m_processShader.loadProgram();
    }

    m_fusionMode = ADDITIVE_COLOR;
    m_useShader = Shader::checkHardware();
}

ColorEffect::~ColorEffect()
{
}

void ColorEffect::process(Rtt* rtt)
{
    m_layer.begin();

    rtt->use(true);

    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(m_useShader)
    {
        rtt->getColor().use(true);
        m_processShader.use(true);
    }
    else
    {
        glClientActiveTexture(GL_TEXTURE0);
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        rtt->getColor().use(true);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glClientActiveTexture(GL_TEXTURE1);
        glActiveTexture(GL_TEXTURE1);
        glEnable(GL_TEXTURE_2D);
        m_fallback.use(true);

        switch(m_fusionMode)
        {
            case ADDITIVE_COLOR: glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
                break;

            case MULTIPLICATION_COLOR: glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                break;

            case SUBTRACTION_COLOR: glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_SUBTRACT);
                break;

            case AVERAGE_COLOR: glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD_SIGNED);
                break;
        }

    }

    m_layer.draw(false);

    if(m_useShader)
    {
        m_processShader.use(false);
        rtt->getColor().use(false);
    }
    else
    {
        glActiveTexture(GL_TEXTURE1);
        glDisable(GL_TEXTURE_2D);
        m_fallback.use(false);

        glActiveTexture(GL_TEXTURE0);
        glDisable(GL_TEXTURE_2D);
        rtt->getColor().use(false);
    }

    glPopAttrib();

    rtt->use(false);

    m_layer.end();
}

void ColorEffect::setFusionMode(ColorEffect::FusionMode fusionMode)
{
    this->m_fusionMode = fusionMode;

    if(Shader::checkHardware())
    {
        m_processShader.use(true);
        m_processShader.uniform("fusionMode", m_fusionMode);
        m_processShader.use(false);
    }
}

ColorEffect::FusionMode ColorEffect::getFusionMode() const
{
    return m_fusionMode;
}

void ColorEffect::setColor(Vector4f color)
{
    this->m_color = color;

    if(Shader::checkHardware())
    {
        m_processShader.use(true);
        m_processShader.uniform("color", m_color);
        m_processShader.use(false);
    }

    m_fallback.remove();
    m_fallback.build(m_workRtt->getFrameSize(), m_color * 255);
}

Vector4f ColorEffect::getColor() const
{
    return m_color;
}

void ColorEffect::setAlpha(float alpha)
{
    m_color.w = alpha;

    if(Shader::checkHardware())
    {
        m_processShader.use(true);
        m_processShader.uniform("color", m_color);
        m_processShader.use(false);
    }

    m_fallback.fill(m_color * 255);
}

float ColorEffect::getAlpha() const
{
    return m_color.w;
}

void ColorEffect::setUseShader(bool useShader)
{
    this->m_useShader = useShader;
}

bool ColorEffect::isUseShader() const
{
    return m_useShader;
}
