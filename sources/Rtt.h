/* 
 * File:   Rtt.h
 * Author: b4n92uid
 *
 * Created on 12 avril 2010, 20:18
 */

#ifndef _RTT_H
#define	_RTT_H

#include "FrameBufferObject.h"
#include "Mathematics.h"

namespace tbe
{

/// \brief Class d'abstraction pour le rendue off-screen

class Rtt
{
public:
    Rtt();
    Rtt(Vector2i frameSize);
    ~Rtt();

    void Use(bool state);

    void Clear();

    void SetCaptureDepth(bool captureDepth);
    bool IsCaptureDepth() const;

    void SetCaptureColor(bool captureColor);
    bool IsCaptureColor() const;

    void SetFrameSize(Vector2i frameSize);
    Vector2i GetFrameSize() const;

    Texture GetColor();
    Texture GetDepht();

    // FBO Methode Functions ---------------------------------------------------

    void FBO_SetMultisamplesCount(unsigned multisamples);
    unsigned FBO_GetMultisamplesCount();

    void FBO_SetUseRenderBuffer(bool state);
    bool FBO_IsUseRenderBuffer();

    void FBO_BlitToTexutre();

    // -------------------------------------------------------------------------

protected:
    Vector2i m_frameSize;
    bool m_captureColor;
    bool m_captureDepth;

private:

    struct
    {
        FrameBufferObject* display;
        FrameBufferObject* render;
        bool useRenderBuffer;
    } m_fboMethod;

    struct
    {
        Texture color;
        Texture depth;
    } m_textureMethod;

    bool m_supportFbo;
};

}

#endif	/* _RTT_H */
