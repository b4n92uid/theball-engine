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

    /// Active la capture d'un composant de l'énumération Compenent
    void attach(unsigned compenent);

    /// Désactive la capture d'un composant de l'énumération Compenent
    void detach(unsigned compenent);

    /**
     * Utilisation du FBO. Le rendue effectuer apres cette appelle
     * sera déssiner sur ce le buffer de cette objet
     *
     * @param use Etat d'utilisation du FBO
     */
    void use(bool state = true);

    /**
     * Déssine le contenue du buffer courrant sur dst avec les conversion
     *  néssésaire a effectuer.
     *
     * @param dst le buffer de déstination
     * @param compenentFlags les compossants a déssiner
     */
    void blit(FrameBufferObject* dst, unsigned compenentFlags = 0);

    /// Vide le buffer courant
    void clear();

    /// Renvois, sous forme de texture, le composant spécifier
    Texture getTexture(Compenent compenent);

    /// Racourci pour l'appelle getTexture(COLOR_TEXTURE_2D)
    Texture getColor();

    /// Spécifier le multi-sampling éffectuer sur le rendue
    void setMultiSamplesCount(unsigned multiSamplesCount);

    /// Renvois le nombre de multi-sampling éffectuer sur le rendue
    unsigned getMultiSamplesCount() const;

    /// Spécifier la taille du FBO
    virtual void setFrameSize(Vector2i frameSize);

    /// Renvois la taille du FBO
    Vector2i getFrameSize() const;

    /**
     * Cette fonction vérifie si le materiel actuelle supporte l'utilisation
     * des FBO (Frame buffer object)
     *
     * Pour déterminer le support des FBO, les Extention suivante sont tester :
     *  GL_EXT_framebuffer_object
     *  GL_EXT_framebuffer_blit
     *  GL_EXT_framebuffer_multisample
     */
    static bool checkHardware();

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

