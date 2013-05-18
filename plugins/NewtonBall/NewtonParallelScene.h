/*
 * File:   NewtonParallelScene.h
 * Author: b4n92uid
 *
 * Created on 27 novembre 2009, 19:37
 */

#ifndef _NewtonParallelScene_H
#define	_NewtonParallelScene_H

#include <Newton.h>
#include <Tbe.h>

#include "NewtonNode.h"

namespace tbe
{
namespace scene
{

/// \brief Scene Parallele Newton
/// Wrapper pour utiliser le moteur physique
/// Newton Game Dynamics avec le moteur TBE

class NewtonParallelScene : public ParallelSceneRegister<NewtonNode>
{
public:
    NewtonParallelScene();
    ~NewtonParallelScene();

    /// Spécifier la gravité appliquer au noeud
    void setGravity(float gravity);
    float getGravity() const;

    /// Rendue
    void render();

    /// Specifier le timestep du moteur physique
    void setWorldTimestep(float worldTimestep);

    /// Accés au timestep du moteur physique
    float getWorldTimestep() const;

    /// Accés au NewtonWorld
    NewtonWorld* getNewtonWorld() const;

    /// Specifier les dimmension du mond physique
    void setWorldSize(AABB wordlSize);
    AABB getWorldSize() const;

    /// Fonction qui recupere la position de collision quand body.masse == 0
    Vector3f findZeroMassBody(Vector3f start, Vector3f end);

    /// Fonction qui recupere la position de collision
    Vector3f findAnyBody(Vector3f start, Vector3f end);

    /// Renvois tout les points d'intersection entre start et end
    Vector3f::Array rayCast(Vector3f start, Vector3f end);

    /// Fonction qui recupere la position sur le sol
    Vector3f findFloor(Vector3f pos);

protected:
    AABB m_worldSize;
    NewtonWorld* m_newtonWorld;
    float m_worldTimestep;
    float m_gravity;
};

}
}

#endif	/* _NewtonParallelScene_H */

