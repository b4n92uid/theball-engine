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
    void SetGravity(float gravity);
    float GetGravity() const;

    /// Rendue
    void Render();

    /// Specifier le timestep du moteur physique
    void SetWorldTimestep(float worldTimestep);

    /// Accés au timestep du moteur physique
    float GetWorldTimestep() const;

    /// Accés au NewtonWorld
    NewtonWorld* GetNewtonWorld() const;

    /// Specifier les dimmension du mond physique
    void SetWorldSize(AABB wordlSize);
    AABB GetWorldSize() const;


    /// Fonction qui recupere la position de collision quand body.masse == 0
    Vector3f FindZeroMassBody(Vector3f start, Vector3f end);

    /// Fonction qui recupere la position de collision
    Vector3f FindAnyBody(Vector3f start, Vector3f end);

    /// Fonction qui recupere la position sur le sol
    Vector3f FindFloor(Vector3f pos);

protected:
    AABB m_worldSize;
    NewtonWorld* m_newtonWorld;
    float m_worldTimestep;
    float m_gravity;
};

}
}

#endif	/* _NewtonParallelScene_H */

