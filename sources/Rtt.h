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

/**
 * \brief Class d'abstraction pour le rendue off-screen
 *
 * Encapsule l'utilisation du FBO et des textures classique
 * pour un rendue adapté selon le materielle
 */

class Rtt
{
public:
    Rtt();
    Rtt(Vector2i frameSize);
    ~Rtt();

    /**
     * Utilisation du rendue hors-écran. Le rendue effectuer apres cette appelle
     * sera déssiner sur ce le buffer de cette objet
     *
     * @param use Etat d'utilisation du rtt
     */
    void use(bool state);

    /// Vide le buffer du rtt
    void clear();

    /// Spécifier l'état de la capture des information relatife a la profondeur
    void setCaptureDepth(bool captureDepth);

    /// Revois l'état de la capture des information relatife a la profondeur
    bool isCaptureDepth() const;

    /// Spécifier l'état de la capture des information relatife a la couleur
    void setCaptureColor(bool captureColor);

    /// Revois l'état de la capture des information relatife a la couleur
    bool isCaptureColor() const;

    /// Spécifier la taille du rtt pour le rendue
    void setFrameSize(Vector2i frameSize);

    /// Renvois la taille du rtt pour le rendue
    Vector2i getFrameSize() const;

    /// Renvois, sous forme de texture, le rendue effectuer sur le buffer
    Texture getColor();

    /// Renvois, sous forme de texture, le rendue effectuer sur le buffer
    /// du tempons de profondeur
    Texture getDepht();

    void setGuiRendring(bool guiRendring);
    bool isGuiRendring() const;

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
    bool m_guiRendring;

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
    Matrix4 m_prevMat;
};

}

#endif	/* _RTT_H */
