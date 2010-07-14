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

    /// Creation de fenetre
    void Window(std::string caption = "theBall Engine",
                Vector2i winsize = Vector2i(800, 600), int bits = 32,
                bool fullscreen = false, int multisamples = 0);

    /// Récuperation des evenements
    void PollEvent();

    /// bloque l'entrer a l'interieur de la fen?tre
    void SetGrabInput(bool stat);

    /// Visibilit? du curseur
    void SetMouseVisible(bool stat);

    /// Efface la scenne
    void BeginScene();

    /// Termine la scene
    void EndScene();

    /// Renvois le degres du Multisampling
    int GetWinMultiSamples() const;

    /// Renvois true su la fenetre est en plein-?cran
    bool IsWinFullscreen() const;

    /// Renvois la profondeur du pixel
    int GetWinBits() const;

    /// Renvois un tableau des rï¿½solution supportï¿½
    static std::vector<Vector2i> GetAvilableSceeenSize();

    /// Renvois le nom de la touche
    static std::string GetKeyName(int keyCode);

    /// Renvois le nom du boutton de la souris
    static std::string GetMouseName(int mouseCode);

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
