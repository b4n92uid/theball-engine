/* 
 * File:   MotionPath.cpp
 * Author: b4n92uid
 * 
 * Created on 14 mai 2010, 20:45
 */

#include "MotionPath.h"

using namespace std;
using namespace tbe;

MotionPath::MotionPath()
{
    m_speed = 1;
    m_progress = 0;
    m_currentIndex = 0;
}

MotionPath::~MotionPath()
{
}

void MotionPath::addKeyFrame(Vector3f pos, float speed)
{
    KeyFrame key = {pos, speed};
    m_keyframe.push_back(key);
}

void MotionPath::update()
{
    if(m_keyframe.empty())
        return;

    if(m_startPos == m_endPos)
    {
        m_currentIndex++;

        m_endPos = m_keyframe[m_currentIndex].pos;
        m_speed = m_keyframe[m_currentIndex].speed;
    }

    m_progress += m_speed;

    m_currentPos = m_startPos + (m_endPos - m_startPos) * m_progress;

    if(m_progress >= 1.0f)
    {
        m_progress = 0;

        m_currentIndex++;

        if(m_currentIndex >= m_keyframe.size())
            m_currentIndex = 0;

        m_startPos = m_endPos;
        m_endPos = m_keyframe[m_currentIndex].pos;
        m_speed = m_keyframe[m_currentIndex].speed;
    }
}

Vector3f MotionPath::getCurrentPos() const
{
    return m_currentPos;
}

