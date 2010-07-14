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

    /// Attache un noeude a la scene
    void AddNode(std::string name, NewtonNode* node);

    /// Acc�s a un Node
    NewtonNode* GetNode(std::string name);

    /// D�trute un noeud
    void DeleteNode(std::string name);

    /// D�tache un noeud du getionnaire
    NewtonNode* ReleaseNode(std::string name);

    /// Sp�cifier si les nodeu devrait �tre d�truit par le d�structeur
    void SetSharedNode(bool sharedNode);
    bool IsSharedNode() const;

    /// Sp�cifier la gravit� appliquer au noeud
    void SetGravity(float gravity);
    float GetGravity() const;

    /// Rendue
    void Render();

    /// Detruit tout les entités
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

    /// Fonction qui recupere la position de collision quand body.masse == 0
    Vector3f FindZeroMassBody(Vector3f start, Vector3f end);

    /// Fonction qui recupere la position de collision
    Vector3f FindAnyBody(Vector3f start, Vector3f end);

    /// Fonction qui recupere la position sur le sol
    Vector3f FindFloor(Vector3f pos);

    /// Method static d'application de la force gravitationnele (9.81)
    static void ApplyForceAndTorque(const NewtonBody* body, float, int);

    // Template ----------------------------------------------------------------

    template <typename T> T* GetNode(std::string name)
    {
        return (T*)GetNode(name);
    }

    template<typename T> T* ReleaseNode(std::string name)
    {
        return (T*)ReleaseNode(name);
    }

protected:
    AABB m_worldSize;
    NewtonWorld* m_newtonWorld;
    NewtonNode::Map m_newtonNodes;
    float m_worldTimestep;
    bool m_sharedNode;
    float m_gravity;
};

}
}

#endif	/* _NewtonParallelScene_H */

