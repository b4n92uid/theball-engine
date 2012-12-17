/*
 * File:   BulletParallelScene.h
 * Author: b4n92uid
 *
 * Created on 27 novembre 2009, 19:37
 */

#ifndef _BULLET_PARALLEL_SCENE_H
#define	_BULLET_PARALLEL_SCENE_H

#include <btBulletDynamicsCommon.h>
#include <Tbe.h>

#include "BulletNode.h"

namespace tbe
{
namespace scene
{

/// \brief Scene Parallele Bullet
/// Wrapper pour utiliser le moteur physique
/// Bullet Physics avec le moteur TBE

class BulletParallelScene : public ParallelSceneRegister<BulletNode>
{
public:
    BulletParallelScene();
    ~BulletParallelScene();

    /// Sp�cifier la gravit� appliquer au noeud
    void setGravity(Vector3f gravity);
    Vector3f getGravity() const;

    /// Rendue
    void render();

    /// Specifier le timestep du moteur physique
    void setWorldTimestep(float worldTimestep);

    /// Acc�s au timestep du moteur physique
    float getWorldTimestep() const;

    /// Acc�s au BulletWorld
    btDiscreteDynamicsWorld* getWorld() const;

protected:
    //  Monde physique
    btDiscreteDynamicsWorld * m_world;
    // La classe btBroadphaseInterface fournit une interface pour d�tecter les objets o� leurs AABB se chevauchent.
    btBroadphaseInterface *m_broadphase;
    // btCollisionDispatcher supporte des algorithmes qui peuvent g�rer des pairs de collisions ConvexConvex et ConvexConcave. Temps de l'impact, le point le plus proche et p�n�tration de profondeur.
    btCollisionDispatcher *m_dispatcher;
    // btCollisionConfiguration permet de configurer les allocataires de m�moire.
    btDefaultCollisionConfiguration *m_collisionConfiguration;
    // btSequentialImpulseConstraintSolver est une impl�mentation SIMD rapide de la m�thode Projected Gauss Seidel (iterative LCP).
    btSequentialImpulseConstraintSolver *m_sequentialImpulseConstraintSolver;

    Vector3f m_gravity;
    float m_worldTimestep;
};

}
}

#endif	/* _BULLET_PARALLEL_SCENE_H */

