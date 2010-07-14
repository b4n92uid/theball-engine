/* 
 * File:   FrameBufferObject.cpp
 * Author: b4n92uid
 * 
 * Created on 22 novembre 2009, 15:53
 */

#include "FrameBufferObject.h"
#include "Texture.h"
#include "Exception.h"

using namespace std;
using namespace tbe;

std::string glStrCheckFramebufferStatus()
{
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);

    switch(status)
    {
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            return "Framebuffer object format is unsupported by the video hardware";
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            return "Incomplete attachment. (GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT)";
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            return "Incomplete missing attachment. (GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT)";
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
                return "Incomplete dimensions. (GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT)";
            case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
                return "Incomplete formats. (GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT)";
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            return "Incomplete draw buffer. (GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT)";
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            return "Incomplete read buffer. (GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT)";
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT:
            return "Incomplete multisample buffer. (GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT)";
        default:
            cout << "Some video driver error or programming error occured. Framebuffer object status is invalid" << endl;
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            return string();
    }
}

// Fbo -------------------------------------------------------------------------

FrameBufferObject::FrameBufferObject()
{
    glGenFramebuffersEXT(1, &m_fboId);

    m_attachedCompenent = 0;

    m_colorRenderBuffer = 0;
    m_depthRenderBuffer = 0;
    m_multiSamplesCount = 0;
}

FrameBufferObject::FrameBufferObject(Vector2i frameSize)
{
    glGenFramebuffersEXT(1, &m_fboId);

    m_attachedCompenent = 0;

    m_colorRenderBuffer = 0;
    m_depthRenderBuffer = 0;
    m_multiSamplesCount = 0;

    m_frameSize = frameSize;
}

FrameBufferObject::FrameBufferObject(const FrameBufferObject& orig)
{
    glGenFramebuffersEXT(1, &m_fboId);

    *this = orig;
}

FrameBufferObject::~FrameBufferObject()
{
    glDeleteFramebuffersEXT(1, &m_fboId);

    if(m_colorTextureId)
        m_colorTextureId.Delete();

    if(m_depthTextureId)
        m_depthTextureId.Delete();

    if(m_colorRenderBuffer)
        glDeleteRenderbuffersEXT(1, &m_colorRenderBuffer);

    if(m_depthRenderBuffer)
        glDeleteRenderbuffersEXT(1, &m_depthRenderBuffer);
}

FrameBufferObject& FrameBufferObject::operator=(const FrameBufferObject& orig)
{
    m_attachedCompenent = orig.m_attachedCompenent;
    m_frameSize = orig.m_frameSize;
    m_multiSamplesCount = orig.m_multiSamplesCount;

    if(m_attachedCompenent)
        Attach(m_attachedCompenent);

    return *this;
}

void FrameBufferObject::Attach(unsigned compenent)
{
    m_attachedCompenent |= compenent;

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboId);

    if(compenent & COLOR_RENDER_BUFFER)
    {
        if(m_colorRenderBuffer)
            glDeleteRenderbuffersEXT(1, &m_colorRenderBuffer);

        // Génération buffer de rendue
        glGenRenderbuffersEXT(1, &m_colorRenderBuffer);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_colorRenderBuffer);
        glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, m_multiSamplesCount, GL_RGBA, m_frameSize.x, m_frameSize.y);

        // Attacher en tant que Color buffer
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, m_colorRenderBuffer);
    }

    if(compenent & DEPTH_RENDER_BUFFER)
    {
        if(m_depthRenderBuffer)
            glDeleteRenderbuffersEXT(1, &m_depthRenderBuffer);

        // Génération buffer de rendue
        glGenRenderbuffersEXT(1, &m_depthRenderBuffer);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depthRenderBuffer);
        glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, m_multiSamplesCount, GL_DEPTH_COMPONENT, m_frameSize.x, m_frameSize.y);

        // Attacher en tant que Depth buffer
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depthRenderBuffer);

    }

    if(compenent & COLOR_TEXTURE_2D)
    {
        if(m_colorTextureId)
            m_colorTextureId.Delete();

        // Génération buffer de rendue
        m_colorTextureId.Build(m_frameSize);

        m_colorTextureId.Use(true);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        m_colorTextureId.Use(false);

        // Attacher en tant que Color buffer
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_colorTextureId, 0);
    }

    if(compenent & DEPTH_TEXTURE_2D)
    {
        if(m_depthTextureId)
            m_depthTextureId.Delete();

        // Génération buffer de rendue
        m_depthTextureId.Build(m_frameSize, 0, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);

        m_depthTextureId.Use(true);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        m_depthTextureId.Use(false);

        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, m_depthTextureId, 0);
    }

    // Check error
    string error = glStrCheckFramebufferStatus();

    if(!error.empty())
        throw tbe::Exception("FboRenderBuffer::Attach; %s", error.c_str());

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void FrameBufferObject::Detach(unsigned compenent)
{
    m_attachedCompenent &= ~compenent;

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboId);

    if(compenent & COLOR_TEXTURE_2D)
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_colorTextureId, 0);

    if(compenent & DEPTH_TEXTURE_2D)
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, m_depthTextureId, 0);

    if(compenent & COLOR_RENDER_BUFFER)
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, 0);

    if(compenent & DEPTH_RENDER_BUFFER)
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);

    glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
}

void FrameBufferObject::Use(bool state)
{
    if(state)
    {
        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(0, 0, m_frameSize.x, m_frameSize.y);

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboId);
    }

    else
    {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

        glPopAttrib();
    }
}

void FrameBufferObject::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if(m_attachedCompenent & DEPTH_TEXTURE_2D
       || m_attachedCompenent & DEPTH_RENDER_BUFFER)
        glClear(GL_DEPTH_BUFFER_BIT);
}

void FrameBufferObject::SetFrameSize(Vector2i frameSize)
{
    this->m_frameSize = frameSize;

    if(unsigned attachedCompenent = m_attachedCompenent)
    {
        Detach(attachedCompenent);
        Attach(attachedCompenent);
    }
}

Vector2i FrameBufferObject::GetFrameSize() const
{
    return m_frameSize;
}

Texture FrameBufferObject::GetTexture(Compenent compenent)
{
    switch(compenent)
    {
        case COLOR_TEXTURE_2D: return m_colorTextureId;
        case DEPTH_TEXTURE_2D: return m_depthTextureId;
        default: return Texture();
    }
}

void FrameBufferObject::Blit(FrameBufferObject* dst, unsigned compenentFlags)
{
    Vector2i srcRect = m_frameSize;
    Vector2i dstRect = dst->m_frameSize;

    if(!compenentFlags)
        compenentFlags = m_attachedCompenent;

    glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, m_fboId);
    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, dst->m_fboId);

    if(compenentFlags & COLOR_RENDER_BUFFER || compenentFlags & COLOR_TEXTURE_2D)
        glBlitFramebufferEXT(0, 0, srcRect.x, srcRect.y, 0, 0, dstRect.x, dstRect.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    if(compenentFlags & DEPTH_RENDER_BUFFER || compenentFlags & DEPTH_TEXTURE_2D)
        glBlitFramebufferEXT(0, 0, srcRect.x, srcRect.y, 0, 0, dstRect.x, dstRect.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
}

void FrameBufferObject::SetMultiSamplesCount(unsigned multiSamplesCount)
{
    this->m_multiSamplesCount = multiSamplesCount;

    if(unsigned attachedCompenent = m_attachedCompenent)
    {
        Detach(attachedCompenent);
        Attach(attachedCompenent);
    }
}

unsigned FrameBufferObject::GetMultiSamplesCount() const
{
    return m_multiSamplesCount;
}

bool FrameBufferObject::CheckHardware()
{
    static bool supported = GLEE_EXT_framebuffer_object && GLEE_EXT_framebuffer_blit && GLEE_EXT_framebuffer_multisample;
    return supported;
}
