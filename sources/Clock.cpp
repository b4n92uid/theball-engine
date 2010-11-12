/* 
 * File:   Clock.cpp
 * Author: b4n92uid
 * 
 * Created on 26 novembre 2009, 22:39
 */

#include "Clock.h"

#include <iostream>
#include <time.h>

#ifdef __WIN32__
#include <windows.h>

#elif defined(__linux__)
#include <unistd.h>
#include <sys/time.h>

#endif

using namespace std;
using namespace tbe;
using namespace tbe::ticks;

Clock::Clock()
{
    m_lastTime = clock();
}

Clock::~Clock()
{
}

void Clock::SnapShoot()
{
    m_lastTime = clock();
}

long Clock::GetEsplanedTime()
{
    long curtime = clock();
    long esplanedTime = curtime - m_lastTime;

    m_lastTime = curtime;

    return esplanedTime;
}

bool Clock::IsEsplanedTime(long time)
{
    long curtime = clock();

    if(curtime - m_lastTime >= time)
        return(m_lastTime = curtime);

    else return false;
}

void Clock::Sleep(unsigned v)
{
    #ifdef __linux__
    usleep(v);

    #elif defined(__WIN32__)
    ::Sleep(v);

    #endif
}
