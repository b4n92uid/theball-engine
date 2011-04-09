/* 
 * File:   Fog.h
 * Author: b4n92uid
 *
 * Created on 23 janvier 2010, 14:33
 */

#ifndef _FOG_H
#define	_FOG_H

#include "GLee.h"

#include "Mathematics.h"

namespace tbe
{
namespace scene
{

/**
 * \brief Gestion du brouiallrd
 */
class Fog
{
public:
    Fog();
    virtual ~Fog();

    void clear();

    void setEnd(float end);
    float getEnd() const;

    void setStart(float start);
    float getStart() const;

    void setDensity(float density);
    float getDensity() const;

    void setColor(Vector4f color);
    Vector4f getColor() const;
    
    void setEnable(bool enable);
    bool isEnable() const;

private:
    bool m_enable;
    Vector4f m_color;
    float m_density;
    float m_start, m_end;
};

}
}

#endif	/* _FOG_H */

