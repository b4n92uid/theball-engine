/* 
 * File:   FPSManager.cpp
 * Author: b4n92uid
 * 
 * Created on 26 novembre 2009, 22:37
 */

#include "FPSManager.h"

using namespace std;
using namespace tbe;
using namespace tbe::ticks;

FpsManager::FpsManager()
{
    m_fps = 0;
    m_betterfps = 0;

    m_framecount = 0;

    m_cyclePerSecond = 0;

    m_lastTimeFramCount = 0;
    m_lastTimeFramShow = 0;

    m_doARender = true;

#ifdef __WIN32__
    QueryPerformanceFrequency((LARGE_INTEGER*) & m_cyclePerSecond);

#elif defined(__linux__)
    m_cyclePerSecond = 1;

#endif

    setRunFps(60);
}

bool FpsManager::doRender()
{
    if(m_doARender)
    {
        m_doARender = false;
        return true;
    }

    else
        return false;
}

void FpsManager::update()
{
    counttype curtime = 0;

#ifdef __WIN32__
    QueryPerformanceCounter((LARGE_INTEGER*) & curtime);

#elif defined(__linux__)
    curtime = clock();

#endif

    if(curtime - m_lastTimeFramShow >= m_timeToRend)
    {
        m_doARender = true;
        m_lastTimeFramShow = curtime;
    }

    if(curtime - m_lastTimeFramCount >= m_cyclePerSecond)
    {
        m_fps = m_framecount;

        if(m_fps > m_betterfps)
            m_betterfps = m_fps;

        m_framecount = 0;
        m_lastTimeFramCount = curtime;
    }

    if(m_doARender)
        m_framecount++;
}

unsigned FpsManager::getCurFps()
{
    return m_fps;
}

unsigned FpsManager::getBetterFps()
{
    return m_betterfps;
}

void FpsManager::setRunFps(unsigned setFps)
{
    this->m_setFps = setFps;
    m_timeToRend = m_cyclePerSecond / m_setFps;
}

unsigned FpsManager::getRunFps() const
{
    return m_setFps;
}
