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

    void Clear();

    void SetEnd(float end);
    float GetEnd() const;

    void SetStart(float start);
    float GetStart() const;

    void SetDensity(float density);
    float GetDensity() const;

    void SetColor(Vector4f color);
    Vector4f GetColor() const;
    
    void SetEnable(bool enable);
    bool IsEnable() const;

private:
    bool m_enable;
    Vector4f m_color;
    float m_density;
    float m_start, m_end;
};

}
}

#endif	/* _FOG_H */

