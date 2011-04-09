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

    Node* clone();

    void process();

    void setFreeMove(float freeMove);
    float getFreeMove() const;

    void setAutoRebuild(bool autoRebuild);
    bool isAutoRebuild() const;

    void setContinousMode(bool continousMode);
    bool isContinousMode() const;

    void setLifeDown(float lifeDown);
    float getLifeDown() const;

    void setLifeInit(float lifeInit);
    float getLifeInit() const;

    void setGravity(Vector3f gravity);
    Vector3f getGravity() const;

    void setBoxSize(Vector3f boxSize);
    Vector3f getBoxSize() const;

    void setEmitPos(Vector3f emitPos);
    Vector3f getEmitPos() const;

    void setBrustCount(int brustCount);
    int getBrustCount() const;

    CtorMap constructionMap(std::string root);

protected:
    void setupBullet(Particle& p);

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

