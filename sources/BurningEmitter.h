/* 
 * File:   BurningEmitter.h
 * Author: b4n92uid
 *
 * Created on 28 mars 2011, 18:51
 */

#ifndef BURNINGEMITTER_H
#define	BURNINGEMITTER_H


#include <vector>

#include "Mathematics.h"
#include "Texture.h"

#include "Node.h"
#include "Material.h"
#include "ParticlesParallelScene.h"
#include "Particles.h"

namespace tbe
{
namespace scene
{

class ParticlesParallelScene;

class BurningEmitter : public ParticlesEmiter
{
public:
    BurningEmitter(ParticlesParallelScene* scene);
    BurningEmitter(const BurningEmitter& copy);
    virtual ~BurningEmitter();

    BurningEmitter & operator=(const BurningEmitter& copy);

    Node* Clone();

    void Process();

    void SetFreeMove(float freeMove);
    float GetFreeMove() const;

    void SetAutoRebuild(bool autoRebuild);
    bool IsAutoRebuild() const;

    void SetContinousMode(bool continousMode);
    bool IsContinousMode() const;

    void SetLifeDown(float lifeDown);
    float GetLifeDown() const;

    void SetLifeInit(float lifeInit);
    float GetLifeInit() const;

    void SetGravity(Vector3f gravity);
    Vector3f GetGravity() const;

    void SetBoxSize(Vector3f boxSize);
    Vector3f GetBoxSize() const;

    void SetEmitPos(Vector3f emitPos);
    Vector3f GetEmitPos() const;

    void SetBrustCount(int brustCount);
    int GetBrustCount() const;

    CtorMap ConstructionMap(std::string root);

protected:
    void SetupBullet(Particle& p);

private:
    float m_lifeInit;
    float m_lifeDown;
    float m_freeMove;

    bool m_continousMode;
    bool m_autoRebuild;

    int m_brustCount;

    Vector3f m_emitPos;
    Vector3f m_gravity;
    Vector3f m_boxSize;
};

}
}

#endif	/* BURNINGEMITTER_H */

