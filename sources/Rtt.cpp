/*
 * File:   Rtt.cpp
 * Author: b4n92uid
 *
 * Created on 12 avril 2010, 20:18
 */

#include "Rtt.h"
#include <GL/glu.h>

using namespace tbe;
using namespace std;

Rtt* Rtt::m_curActive = NULL;

Rtt::Rtt()
{
    m_prevActive = NULL;

    m_captureColor = false;
    m_captureDepth = false;

    m_guiRendring = false;

    m_supportFbo = FrameBufferObject::checkHardware();

    // Fbo
    if(m_supportFbo)
    {
        m_fboMethod.render = new FrameBufferObject;
        m_fboMethod.display = new FrameBufferObject;
        m_fboMethod.useRenderBuffer = false;
    }

    else
    {
        m_fboMethod.render = NULL;
        m_fboMethod.display = NULL;
        m_fboMethod.useRenderBuffer = false;
    }
}

Rtt::Rtt(Vector2i frameSize)
{
    m_prevActive = NULL;

    m_captureColor = false;
    m_captureDepth = false;

    m_guiRendring = false;

    m_supportFbo = FrameBufferObject::checkHardware();

    m_frameSize = frameSize;

    // Fbo
    if(m_supportFbo)
    {
        m_fboMethod.render = new FrameBufferObject;
        m_fboMethod.render->setFrameSize(frameSize);

        m_fboMethod.display = new FrameBufferObject;
        m_fboMethod.display->setFrameSize(frameSize);

        m_fboMethod.useRenderBuffer = false;
    }

    else
    {
        m_fboMethod.render = NULL;
        m_fboMethod.display = NULL;
        m_fboMethod.useRenderBuffer = false;
    }

    // Texture
    m_textureMethod.color.build(frameSize);
    m_textureMethod.depth.build(frameSize, 0, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
}

Rtt::~Rtt()
{
    // Fbo
    if(m_supportFbo)
    {
        delete m_fboMethod.render;
        delete m_fboMethod.display;
    }

    // Texture
    m_textureMethod.color.release();
    m_textureMethod.depth.release();
}

void Rtt::use(bool state)
{
    if(m_supportFbo)
    {
        if(state)
        {
            if(m_curActive)
            {
                m_prevActive = m_curActive;
                m_curActive->use(false);

                glGetFloatv(GL_MODELVIEW_MATRIX, m_prevMat);
                glPopMatrix();
            }

            m_curActive = this;

            m_fboMethod.useRenderBuffer
                    ? m_fboMethod.render->use(true)
                    : m_fboMethod.display->use(true);
        }

        else
        {
            m_fboMethod.useRenderBuffer
                    ? m_fboMethod.render->use(false)
                    : m_fboMethod.display->use(false);

            m_curActive = NULL;

            if(m_prevActive)
            {
                glLoadMatrixf(m_prevMat);
                glPushMatrix();

                m_prevActive->use(true);
                m_prevActive = NULL;
            }
        }
    }

    else
    {
        if(state)
        {
            glPushAttrib(GL_VIEWPORT_BIT);

            if(m_guiRendring)
            {
                gluOrtho2D(0, m_frameSize.x, 0, m_frameSize.y);

                glPushMatrix();
                glLoadIdentity();
            }
            else
                glViewport(0, 0, m_frameSize.x, m_frameSize.y);
        }

        else
        {
            if(m_captureColor)
            {
                m_textureMethod.color.use(true);
                glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, m_frameSize.x, m_frameSize.y, 0);
                m_textureMethod.color.use(false);
            }

            if(m_captureDepth)
            {
                m_textureMethod.depth.use(true);
                glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, m_frameSize.x, m_frameSize.y, 0);
                m_textureMethod.depth.use(false);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if(m_guiRendring)
                glPopMatrix();

            glPopAttrib();
        }
    }
}

void Rtt::clear()
{
    if(m_supportFbo)
    {
        if(m_fboMethod.useRenderBuffer)
            m_fboMethod.render->clear();
        else
            m_fboMethod.display->clear();
    }

    else
    {
        //        m_textureMethod.color.Fill(0);
        //        m_textureMethod.depth.Fill(0);
    }
}

void Rtt::setFrameSize(Vector2i frameSize)
{
    this->m_frameSize = frameSize;

    // Fbo
    if(m_supportFbo)
    {
        m_fboMethod.display->setFrameSize(frameSize);
        m_fboMethod.render->setFrameSize(frameSize);
    }

    // Teture
    m_textureMethod.color.release();
    m_textureMethod.color.build(frameSize);

    m_textureMethod.depth.release();
    m_textureMethod.depth.build(frameSize, 0, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
}

Vector2i Rtt::getFrameSize() const
{
    return m_frameSize;
}

void Rtt::setCaptureDepth(bool captureDepth)
{
    this->m_captureDepth = captureDepth;

    if(m_supportFbo)
    {
        if(m_captureDepth)
        {
            m_fboMethod.display->attach(FrameBufferObject::DEPTH_TEXTURE_2D);
            m_fboMethod.render->attach(FrameBufferObject::DEPTH_RENDER_BUFFER);
        }
        else
        {
            m_fboMethod.display->detach(FrameBufferObject::DEPTH_TEXTURE_2D);
            m_fboMethod.render->detach(FrameBufferObject::DEPTH_RENDER_BUFFER);
        }
    }
}

bool Rtt::isCaptureDepth() const
{
    return m_captureDepth;
}

void Rtt::setCaptureColor(bool captureColor)
{
    this->m_captureColor = captureColor;

    if(m_supportFbo)
    {
        if(m_captureColor)
        {
            m_fboMethod.display->attach(FrameBufferObject::COLOR_TEXTURE_2D);
            m_fboMethod.render->attach(FrameBufferObject::COLOR_RENDER_BUFFER);
        }
        else
        {
            m_fboMethod.display->detach(FrameBufferObject::COLOR_TEXTURE_2D);
            m_fboMethod.render->detach(FrameBufferObject::COLOR_RENDER_BUFFER);
        }
    }
}

bool Rtt::isCaptureColor() const
{
    return m_captureColor;
}

Texture Rtt::getColor()
{
    if(m_supportFbo)
        return m_fboMethod.display->getTexture(FrameBufferObject::COLOR_TEXTURE_2D);
    else
        return m_textureMethod.color;
}

Texture Rtt::getDepht()
{
    if(m_supportFbo)
        return m_fboMethod.display->getTexture(FrameBufferObject::DEPTH_TEXTURE_2D);
    else
        return m_textureMethod.depth;
}

// FBO Methode -----------------------------------------------------------------

void Rtt::FBO_SetMultisamplesCount(unsigned multisamples)
{
    m_fboMethod.render->setMultiSamplesCount(multisamples);
}

unsigned Rtt::FBO_GetMultisamplesCount()
{
    return m_fboMethod.render->getMultiSamplesCount();
}

void Rtt::FBO_SetUseRenderBuffer(bool state)
{
    m_fboMethod.useRenderBuffer = state;
}

bool Rtt::FBO_IsUseRenderBuffer()
{
    return m_fboMethod.useRenderBuffer;
}

void Rtt::FBO_BlitToTexutre()
{
    m_fboMethod.render->blit(m_fboMethod.display);
}

void Rtt::setGuiRendring(bool guiRendring)
{
    if(m_supportFbo)
    {
        m_fboMethod.display->setOrthoCatch(guiRendring);
        m_fboMethod.render->setOrthoCatch(guiRendring);
    }

    this->m_guiRendring = guiRendring;
}

bool Rtt::isGuiRendring() const
{
    return m_guiRendring;
}
