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
        "void main(void)"
        "{"
        "gl_TexCoord[0] = gl_MultiTexCoord0;"
        "gl_Position = ftransform();"
        "}";

static const char fragmentShader[] =
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
        "final = (color + final) / 2;"

        "else if(fusionMode == 5)"
        "{"
        "vec4 set = final * color;"
        "final = (set.r + set.g + set.b) / 3.0f;"
        "}"

        "else if(fusionMode == 6)"
        "final = (1.0f - final);"

        "final.a = color.a;"

        "gl_FragColor = final;"
        "}";

ColorEffect::ColorEffect()
{
    m_processShader.ParseFragmentShader(fragmentShader);
    m_processShader.ParseVertexShader(vertexShader);
    m_processShader.LoadProgram();

    m_fusionMode = BLACK_WHITE;
    m_internalPass = false;
}

ColorEffect::~ColorEffect()
{
}

void ColorEffect::Process(Rtt* rtt)
{
    if(m_internalPass)
    {
        m_workRtt->Use(true);

        m_processShader.Use(true);
        rtt->GetColor().Use(true);

        m_layer.Draw();

        rtt->GetColor().Use(false);
        m_processShader.Use(false);

        m_workRtt->Use(false);

        rtt->Use(true);

        glPushAttrib(GL_ENABLE_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_workRtt->GetColor().Use(true);
        m_layer.Draw();
        m_workRtt->GetColor().Use(false);

        glPopAttrib();

        rtt->Use(false);
    }

    else
    {
        rtt->Use(true);
        m_processShader.Use(true);
        rtt->GetColor().Use(true);

        glPushAttrib(GL_ENABLE_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_layer.Draw();

        glPopAttrib();

        rtt->GetColor().Use(false);
        m_processShader.Use(false);
        rtt->Use(false);
    }
}

void ColorEffect::SetFusionMode(ColorEffect::FusionMode fusionMode)
{
    this->m_fusionMode = fusionMode;

    m_processShader.Use(true);
    m_processShader.SetUniform("fusionMode", m_fusionMode);
    m_processShader.Use(false);
}

ColorEffect::FusionMode ColorEffect::GetFusionMode() const
{
    return m_fusionMode;
}

void ColorEffect::SetColor(Vector4f color)
{
    this->m_color = color;

    m_processShader.Use(true);
    m_processShader.SetUniform("color", m_color);
    m_processShader.Use(false);
}

Vector4f ColorEffect::GetColor() const
{
    return m_color;
}
void ColorEffect::SetInternalPass(bool internalPass)
{
    this->m_internalPass = internalPass;
}
bool ColorEffect::IsInternalPass() const
{
    return m_internalPass;
}
