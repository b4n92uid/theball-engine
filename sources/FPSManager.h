/* 
 * File:   FPSManager.h
 * Author: b4n92uid
 *
 * Created on 26 novembre 2009, 22:37
 */

#ifndef _FPSMANAGER_H
#define	_FPSMANAGER_H

#include <iostream>
#include <string>

#ifdef __WIN32__
#include <windows.h>

#elif defined(__linux__)

#endif

namespace tbe
{
namespace ticks
{

/**
 * \brief Gestionnaire FPS
 * Pour la l'affichage et la limitation du framerate
 */
class FpsManager
{
public:
    #ifdef __WIN32__
    typedef LONGLONG counttype;

    #elif defined(__linux__)
    typedef unsigned long long counttype;

    #endif

    FpsManager();

    /// Renvois true si un rendue doit etre éffécture
    ///  pour respecter le framerate spécifier
    bool doRender();

    /// Met a jour le FPS courant
    void update();

    /// Renvoi le FPS courant
    unsigned getCurFps();

    /// Renvoi le meilleur FPS enregistrer
    unsigned getBetterFps();

    /// Specifier le framerate de l'application
    void setRunFps(unsigned setFps);

    /// Renvois le framerate specifier de l'application
    unsigned getRunFps() const;

    counttype getFromLastRender() const;
    double getFromLastRenderMs() const;
    
    counttype getFromLastUpdate() const;
    double getFromLastFrameMs() const;

protected:

    unsigned m_setFps;

    unsigned m_fps;
    unsigned m_betterfps;

    unsigned m_framecount;

    counttype m_cyclePerSecond;
    counttype m_timeToRend;

    counttype m_updateTick;
    counttype m_secondTick;
    counttype m_renderTick;

    bool m_doARender;
};

}
}

#endif	/* _FPSMANAGER_H */

