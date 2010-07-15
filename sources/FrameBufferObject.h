/* 
 * File:   FrameBufferObject.h
 * Author: b4n92uid
 *
 * Created on 22 novembre 2009, 15:53
 */

#ifndef _FRAMEBUFFEROBJECT_H
#define	_FRAMEBUFFEROBJECT_H

#include "Mathematics.h"
#include "Texture.h"

namespace tbe
{

/// \brief Class de manibulation des Frame buffer object

class FrameBufferObject
{
public:
    FrameBufferObject();
    FrameBufferObject(Vector2i frameSize);
    FrameBufferObject(const FrameBufferObject& orig);
    virtual ~FrameBufferObject();

    FrameBufferObject & operator=(const FrameBufferObject& orig);

    enum Compenent
    {
        COLOR_RENDER_BUFFER = 2,
        DEPTH_RENDER_BUFFER = 4,
        COLOR_TEXTURE_2D = 8,
        DEPTH_TEXTURE_2D = 16,
    };

    void Attach(unsigned compenent);
    void Detach(unsigned compenent);

    void Blit(FrameBufferObject* dst, unsigned compenentFlags = 0);

    void SetMultiSamplesCount(unsigned multiSamplesCount);
    unsigned GetMultiSamplesCount() const;

    Texture GetTexture(Compenent compenent);

    Texture GetColor()
    {
        return GetTexture(FrameBufferObject::COLOR_TEXTURE_2D);
    }

    virtual void SetFrameSize(Vector2i frameSize);
    Vector2i GetFrameSize() const;

    void Clear();

    void Use(bool state = true);

    static bool CheckHardware();

protected:
    unsigned m_attachedCompenent;
    Vector2i m_frameSize;
    GLuint m_fboId;

    GLuint m_depthRenderBuffer;
    GLuint m_colorRenderBuffer;
    unsigned m_multiSamplesCount;

    Texture m_colorTextureId;
    Texture m_depthTextureId;
};

}

#endif	/* _FRAMEBUFFEROBJECT_H */

