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

        "void main(void)"
        "{"

        "float offset = 0.0078125;"

        "vec4 final = texture2D(texture, gl_TexCoord[0].st);"
        "final += texture2D(texture, gl_TexCoord[0].st + vec2(offset,0));"
        "final += texture2D(texture, gl_TexCoord[0].st + vec2(-offset,0));"
        "final += texture2D(texture, gl_TexCoord[0].st + vec2(0,offset));"
        "final += texture2D(texture, gl_TexCoord[0].st + vec2(0,-offset));"
        "final += texture2D(texture, gl_TexCoord[0].st + vec2(offset,offset));"
        "final += texture2D(texture, gl_TexCoord[0].st + vec2(offset,-offset));"
        "final += texture2D(texture, gl_TexCoord[0].st + vec2(-offset,-offset));"
        "final += texture2D(texture, gl_TexCoord[0].st + vec2(-offset,offset));"

        "final /= vec4(9.0);"

        "gl_FragColor = final;"

        "}";

BlurEffect::BlurEffect()
{
    m_processShader.parseFragmentShader(fragmentShader);
    m_processShader.parseVertexShader(vertexShader);
    m_processShader.loadProgram();

    setPasse(1);
}

BlurEffect::~BlurEffect()
{
}

void BlurEffect::process(Rtt* rtt)
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
        rtt->use(true);
        m_processShader.use(true);

        rtt->getColor().use(true);
        m_layer.draw();

        m_processShader.use(false);
        rtt->use(false);
    }

    else
    {
        // Etape 1

        m_processShader.use(true);

        m_workRtt->use(true);

        rtt->getColor().use(true);
        m_layer.draw();

        m_workRtt->use(false);

        // Etape 2

        m_workRtt->use(true);

        m_workRtt->getColor().use(true);
        for(unsigned i = 1; i < m_passe; i++)
            m_layer.draw();

        m_workRtt->use(false);

        m_processShader.use(false);

        // Etape 3

        rtt->use(true);

        m_workRtt->getColor().use(true);
        m_layer.draw();

        rtt->use(false);
    }
}

void BlurEffect::setPasse(unsigned passe)
{
    this->m_passe = passe;
}

unsigned BlurEffect::getPasse() const
{
    return m_passe;
}
