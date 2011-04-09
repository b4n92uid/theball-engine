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

    void use(bool state);

    void clear();

    void setCaptureDepth(bool captureDepth);
    bool isCaptureDepth() const;

    void setCaptureColor(bool captureColor);
    bool isCaptureColor() const;

    void setFrameSize(Vector2i frameSize);
    Vector2i getFrameSize() const;

    Texture getColor();
    Texture getDepht();

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

    Rtt* m_prevActive;
    static Rtt* m_curActive;
    Matrix4f m_prevMat;
};

}

#endif	/* _RTT_H */
