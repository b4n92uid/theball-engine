/*
 * File:   BlurEffect.cpp
 * Author: b4n92uid
 *
 * Created on 4 janvier 2010, 15:06
 */

#include "BlurEffect.h"

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
        "uniform float offset;"

        "void main(void)"
        "{"

        "vec4 final = texture2D(texture,gl_TexCoord[0].st);"

        "final +=  texture2D(texture, gl_TexCoord[0].st+vec2(offset,0));"
        "final +=  texture2D(texture, gl_TexCoord[0].st+vec2(-offset,0));"
        "final +=  texture2D(texture, gl_TexCoord[0].st+vec2(0,offset));"
        "final +=  texture2D(texture, gl_TexCoord[0].st+vec2(0,-offset));"
        "final +=  texture2D(texture, gl_TexCoord[0].st+vec2(offset,offset));"
        "final +=  texture2D(texture, gl_TexCoord[0].st+vec2(offset,-offset));"
        "final +=  texture2D(texture, gl_TexCoord[0].st+vec2(-offset,-offset));"
        "final +=  texture2D(texture, gl_TexCoord[0].st+vec2(-offset,offset));"

        "final /= 9.0;"

        "gl_FragColor = final;"
        "}";

BlurEffect::BlurEffect()
{
    m_processShader.ParseFragmentShader(fragmentShader);
    m_processShader.ParseVertexShader(vertexShader);
    m_processShader.LoadProgram();

    SetPasse(1);
    SetOffset(1.0 / 128.0);
}

BlurEffect::~BlurEffect()
{
}

void BlurEffect::Process(Rtt* rtt)
{
    /*
     * Etape du blur :
     * ---------------
     * Rendue input -> workFbo
     * Passe de blur sur workFbo
     * Rendue workFbo -> output
     */

    if(m_passe == 1) // Optimisation
    {
        rtt->Use(true);
        m_processShader.Use(true);

        rtt->GetColor().Use(true);
        m_layer.Draw();

        m_processShader.Use(false);
        rtt->Use(false);
    }

    else
    {
        // Etape 1

        m_processShader.Use(true);
        
        m_workRtt->Use(true);

        rtt->GetColor().Use(true);
        m_layer.Draw();

        m_workRtt->Use(false);

        // Etape 2

        m_workRtt->Use(true);

        m_workRtt->GetColor().Use(true);
        for(unsigned i = 1; i < m_passe; i++)
            m_layer.Draw();

        m_workRtt->Use(false);

        m_processShader.Use(false);

        // Etape 3

        rtt->Use(true);

        m_workRtt->GetColor().Use(true);
        m_layer.Draw();

        rtt->Use(false);
    }
}

void BlurEffect::SetPasse(unsigned passe)
{
    this->m_passe = passe;
}

unsigned BlurEffect::GetPasse() const
{
    return m_passe;
}

void BlurEffect::SetOffset(float offset)
{
    this->m_offset = offset;

    m_processShader.Use(true);
    m_processShader.SetUniform("offset", m_offset);
    m_processShader.Use(false);
}

float BlurEffect::GetOffset() const
{
    return m_offset;
}
