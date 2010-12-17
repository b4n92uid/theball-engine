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

class ParallelScene;

/// \brief Scene Parallele Newton
/// Wrapper pour utiliser le moteur physique
/// Newton Game Dynamics avec le moteur TBE

class NewtonParallelScene : public ParallelScene
{
public:
    NewtonParallelScene();
    ~NewtonParallelScene();

    /// Sp�cifier la gravit� appliquer au noeud
    void SetGravity(float gravity);
    float GetGravity() const;

    /// Rendue
    void Render();

    void Clear();

    /// Specifier le timestep du moteur physique
    void SetWorldTimestep(float worldTimestep);

    /// Acc�s au timestep du moteur physique
    float GetWorldTimestep() const;

    /// Acc�s au NewtonWorld
    NewtonWorld* GetNewtonWorld() const;

    /// Specifier les dimmension du mond physique
    void SetWorldSize(AABB wordlSize);
    AABB GetWorldSize() const;

    void RegisterBody(NewtonNode* body);
    void UnRegisterBody(NewtonNode* body, bool delptr = false);

    /// Fonction qui recupere la position de collision quand body.masse == 0
    Vector3f FindZeroMassBody(Vector3f start, Vector3f end);

    /// Fonction qui recupere la position de collision
    Vector3f FindAnyBody(Vector3f start, Vector3f end);

    /// Fonction qui recupere la position sur le sol
    Vector3f FindFloor(Vector3f pos);

    /// Method static d'application de la force gravitationnele (9.81)
    static void ApplyForceAndTorque(const NewtonBody* body, float, int);

protected:
    AABB m_worldSize;
    NewtonWorld* m_newtonWorld;
    float m_worldTimestep;
    float m_gravity;
    NewtonNode::Array m_nodes;
};

}
}

#endif	/* _NewtonParallelScene_H */

