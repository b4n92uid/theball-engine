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
    m_processShader.parseFragmentShader(fragmentShader);
    m_processShader.parseVertexShader(vertexShader);
    m_processShader.loadProgram();

    m_fusionMode = BLACK_WHITE;
    m_internalPass = false;
}

ColorEffect::~ColorEffect()
{
}

void ColorEffect::process(Rtt* rtt)
{
    if(m_internalPass)
    {
        m_workRtt->use(true);

        m_processShader.use(true);
        rtt->getColor().use(true);

        m_layer.draw();

        rtt->getColor().use(false);
        m_processShader.use(false);

        m_workRtt->use(false);

        rtt->use(true);

        glPushAttrib(GL_ENABLE_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_workRtt->getColor().use(true);
        m_layer.draw();
        m_workRtt->getColor().use(false);

        glPopAttrib();

        rtt->use(false);
    }

    else
    {
        rtt->use(true);
        m_processShader.use(true);
        rtt->getColor().use(true);

        glPushAttrib(GL_ENABLE_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_layer.draw();

        glPopAttrib();

        rtt->getColor().use(false);
        m_processShader.use(false);
        rtt->use(false);
    }
}

void ColorEffect::setFusionMode(ColorEffect::FusionMode fusionMode)
{
    this->m_fusionMode = fusionMode;

    m_processShader.use(true);
    m_processShader.uniform("fusionMode", m_fusionMode);
    m_processShader.use(false);
}

ColorEffect::FusionMode ColorEffect::getFusionMode() const
{
    return m_fusionMode;
}

void ColorEffect::setColor(Vector4f color)
{
    this->m_color = color;

    m_processShader.use(true);
    m_processShader.uniform("color", m_color);
    m_processShader.use(false);
}

Vector4f ColorEffect::getColor() const
{
    return m_color;
}
void ColorEffect::setInternalPass(bool internalPass)
{
    this->m_internalPass = internalPass;
}
bool ColorEffect::isInternalPass() const
{
    return m_internalPass;
}
