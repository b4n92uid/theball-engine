/* 
 * File:   Rtt.cpp
 * Author: b4n92uid
 * 
 * Created on 12 avril 2010, 20:18
 */

#include "Rtt.h"

using namespace tbe;
using namespace std;

Rtt* Rtt::m_curActive = NULL;

Rtt::Rtt()
{
    m_prevActive = NULL;

    m_captureColor = false;
    m_captureDepth = false;

    m_supportFbo = FrameBufferObject::CheckHardware();

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

    m_supportFbo = FrameBufferObject::CheckHardware();

    m_frameSize = frameSize;

    // Fbo
    if(m_supportFbo)
    {
        m_fboMethod.render = new FrameBufferObject;
        m_fboMethod.render->SetFrameSize(frameSize);

        m_fboMethod.display = new FrameBufferObject;
        m_fboMethod.display->SetFrameSize(frameSize);

        m_fboMethod.useRenderBuffer = false;
    }

    else
    {
        m_fboMethod.render = NULL;
        m_fboMethod.display = NULL;
        m_fboMethod.useRenderBuffer = false;
    }

    // Texture
    m_textureMethod.color.Build(frameSize);
    m_textureMethod.depth.Build(frameSize, 0, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
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
    m_textureMethod.color.Release();
    m_textureMethod.depth.Release();
}

void Rtt::Use(bool state)
{
    if(m_supportFbo)
    {
        if(state)
        {
            if(m_curActive)
            {
                m_prevActive = m_curActive;
                m_curActive->Use(false);

                glGetFloatv(GL_MODELVIEW_MATRIX, m_prevMat);
                glPopMatrix();
            }

            m_curActive = this;

            m_fboMethod.useRenderBuffer
                    ? m_fboMethod.render->Use(true)
                    : m_fboMethod.display->Use(true);
        }

        else
        {
            m_fboMethod.useRenderBuffer
                    ? m_fboMethod.render->Use(false)
                    : m_fboMethod.display->Use(false);

            m_curActive = NULL;

            if(m_prevActive)
            {
                glLoadMatrixf(m_prevMat);
                glPushMatrix();

                m_prevActive->Use(true);
                m_prevActive = NULL;
            }
        }
    }

    else
    {
        if(state)
        {
            glPushAttrib(GL_VIEWPORT_BIT);
            glViewport(0, 0, m_frameSize.x, m_frameSize.y);
        }

        else
        {
            if(m_captureColor)
            {
                m_textureMethod.color.Use(true);
                glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, m_frameSize.x, m_frameSize.y, 0);
                m_textureMethod.color.Use(false);
            }

            if(m_captureDepth)
            {
                m_textureMethod.depth.Use(true);
                glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, m_frameSize.x, m_frameSize.y, 0);
                m_textureMethod.depth.Use(false);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPopAttrib();
        }
    }
}

void Rtt::Clear()
{
    if(m_supportFbo)
    {
        if(m_fboMethod.useRenderBuffer)
            m_fboMethod.render->Clear();
        else
            m_fboMethod.display->Clear();
    }

    else
    {
        //        m_textureMethod.color.Fill(0);
        //        m_textureMethod.depth.Fill(0);
    }
}

void Rtt::SetFrameSize(Vector2i frameSize)
{
    this->m_frameSize = frameSize;

    // Fbo
    if(m_supportFbo)
    {
        m_fboMethod.display->SetFrameSize(frameSize);
        m_fboMethod.render->SetFrameSize(frameSize);
    }

    // Teture
    m_textureMethod.color.Release();
    m_textureMethod.color.Build(frameSize);

    m_textureMethod.depth.Release();
    m_textureMethod.depth.Build(frameSize, 0, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
}

Vector2i Rtt::GetFrameSize() const
{
    return m_frameSize;
}

void Rtt::SetCaptureDepth(bool captureDepth)
{
    this->m_captureDepth = captureDepth;

    if(m_supportFbo)
    {
        if(m_captureDepth)
        {
            m_fboMethod.display->Attach(FrameBufferObject::DEPTH_TEXTURE_2D);
            m_fboMethod.render->Attach(FrameBufferObject::DEPTH_RENDER_BUFFER);
        }
        else
        {
            m_fboMethod.display->Detach(FrameBufferObject::DEPTH_TEXTURE_2D);
            m_fboMethod.render->Detach(FrameBufferObject::DEPTH_RENDER_BUFFER);
        }
    }
}

bool Rtt::IsCaptureDepth() const
{
    return m_captureDepth;
}

void Rtt::SetCaptureColor(bool captureColor)
{
    this->m_captureColor = captureColor;

    if(m_supportFbo)
    {
        if(m_captureColor)
        {
            m_fboMethod.display->Attach(FrameBufferObject::COLOR_TEXTURE_2D);
            m_fboMethod.render->Attach(FrameBufferObject::COLOR_RENDER_BUFFER);
        }
        else
        {
            m_fboMethod.display->Detach(FrameBufferObject::COLOR_TEXTURE_2D);
            m_fboMethod.render->Detach(FrameBufferObject::COLOR_RENDER_BUFFER);
        }
    }
}

bool Rtt::IsCaptureColor() const
{
    return m_captureColor;
}

Texture Rtt::GetColor()
{
    if(m_supportFbo)
        return m_fboMethod.display->GetTexture(FrameBufferObject::COLOR_TEXTURE_2D);
    else
        return m_textureMethod.color;
}

Texture Rtt::GetDepht()
{
    if(m_supportFbo)
        return m_fboMethod.display->GetTexture(FrameBufferObject::DEPTH_TEXTURE_2D);
    else
        return m_textureMethod.depth;
}

// FBO Methode -----------------------------------------------------------------

void Rtt::FBO_SetMultisamplesCount(unsigned multisamples)
{
    m_fboMethod.render->SetMultiSamplesCount(multisamples);
}

unsigned Rtt::FBO_GetMultisamplesCount()
{
    return m_fboMethod.render->GetMultiSamplesCount();
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
    m_fboMethod.render->Blit(m_fboMethod.display);
}
