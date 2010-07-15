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

    /// Ctor
    FpsManager();

    /// ...
    bool DoARender();

    /// Met a jour le FPS courant
    void Update();

    /// Renvoi le FPS courant
    unsigned GetCurFps();

    /// Renvoi le meilleur FPS enregistrer
    unsigned GetBetterFps();

    /// Specifier le framerate de l'application
    void SetRunFps(unsigned setFps);

    /// Renvois le framerate specifier de l'application
    unsigned GetRunFps() const;

protected:

    #ifdef __WIN32__
    typedef LONGLONG counttype;

    #elif defined(__linux__)
    typedef unsigned long long counttype;

    #endif

    unsigned m_setFps;

    unsigned m_fps;
    unsigned m_betterfps;

    unsigned m_framecount;

    counttype m_cyclePerSecond;
    counttype m_timeToRend;

    counttype m_lastTimeFramCount;
    counttype m_lastTimeFramShow;

    bool m_doARender;
};

}
}

#endif	/* _FPSMANAGER_H */

