/* 
 * File:   SDLDevice.h
 * Author: b4n92uid
 *
 * Created on 15 mai 2010, 22:24
 */

#ifndef _SDLDEVICE_H
#define	_SDLDEVICE_H

#include <Tbe.h>

#include <SDL.h>
#undef main

namespace tbe
{

class SDLDevice : public Device
{
public:
    SDLDevice();
    ~SDLDevice();

    /// Céeation de fenetre
    void window(std::string caption = "theBall Engine",
                Vector2i winsize = Vector2i(800, 600), int bits = 32,
                bool fullscreen = false, int multisamples = 0);

    /// Mode vidéo
    void setVideoMode(Vector2i winsize, int bits, bool fullscreen, int multisamples);

    /// Récuperation des evenements
    void pollEvent();

    /// bloque l'entrer a l'interieur de la fen?tre
    void setGrabInput(bool stat);

    /// Visibilité du curseur
    void setMouseVisible(bool stat);

    /// Efface la scenne
    void beginScene();

    /// Termine la scene
    void endScene();

    /// Renvois le degres du Multisampling
    int getWinMultiSamples() const;

    /// Renvois true su la fenetre est en plein-?cran
    bool isWinFullscreen() const;

    /// Renvois la profondeur du pixel
    int getWinBits() const;

    /// Renvois un tableau des résolutions supporter
    static std::vector<Vector2i> getAvilableSceeenSize();

    /// Renvois le nom de la touche
    static std::string getKeyName(int keyCode);

    /// Renvois le nom du boutton de la souris
    static std::string getMouseName(int mouseCode);

private:
    std::string m_caption;
    bool m_mouseGrab;
    bool m_mouseVisible;
    bool m_winFullscreen;
    int m_winMultiSamples;
    int m_winBits;

    std::map<int, int> m_keyStateTmp;
};

}

#endif	/* _SDLDEVICE_H */
