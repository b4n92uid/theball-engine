/* 
 * File:   MotionPath.h
 * Author: b4n92uid
 *
 * Created on 14 mai 2010, 20:45
 */

#ifndef _MOTIONPATH_H
#define	_MOTIONPATH_H

#include <vector>
#include "Mathematics.h"

namespace tbe
{

/// \brief Gestion des chemin de mouvement linéariser par key-frame

class MotionPath
{
public:
    MotionPath();
    ~MotionPath();

    void addKeyFrame(Vector3f pos, float speed = 1.0f);
    void update();
    Vector3f getCurrentPos() const;

protected:

    struct KeyFrame
    {
        Vector3f pos;
        float speed;
        typedef std::vector<KeyFrame> Array;
    };

    KeyFrame::Array m_keyframe;
    unsigned m_currentIndex;

private:
    float m_speed;
    float m_progress;
    Vector3f m_startPos;
    Vector3f m_currentPos;
    Vector3f m_endPos;
};

}

#endif	/* _MOTIONPATH_H */
