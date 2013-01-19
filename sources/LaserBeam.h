/*
 * File:   LaserBeam.h
 * Author: b4n92uid
 *
 * Created on 18 janvier 2013, 20:46
 */

#ifndef LASERBEAM_H
#define	LASERBEAM_H

#include "Mesh.h"

namespace tbe
{
namespace scene
{

class MeshParallelScene;

class LaserBeam : public Mesh
{
public:
    LaserBeam(MeshParallelScene* scene);
    ~LaserBeam();

    void shoot(Vector3f start, Vector3f end);

    void setTexture(Texture texture);
    Texture getTexture() const;

    void setOpacity(float opacity);
    float getOpacity() const;

    Vector3f getEnd() const;
    Vector3f getStart() const;

private:
    Vector3f m_start;
    Vector3f m_end;
    float m_opacity;
    Texture m_texture;
};

}
}

#endif	/* LASERBEAM_H */
