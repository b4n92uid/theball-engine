#ifndef _TBE_PARTICLES_H
#define _TBE_PARTICLES_H

#include <vector>

#include "Mathematics.h"
#include "Texture.h"

#include "Node.h"
#include "Material.h"
#include "ParticlesParallelScene.h"
#include "Clock.h"

namespace tbe
{
namespace scene
{

class ParticlesParallelScene;

/**
 * \brief Représentation d'une particule
 */
struct Particle
{
    typedef std::vector<Particle> Array;

    Vector3f pos;
    Vector4f color;
    Vector3f diriction;
    Vector3f gravity;
    float life;
};

/**
 * \brief Emitter de particules.
 * Encapsule tout les propriétés de génération
 */
class ParticlesEmiter : public Node
{
public:

    typedef std::map<std::string, ParticlesEmiter*> Map;
    typedef std::vector<ParticlesEmiter*> Array;

    ParticlesEmiter(ParticlesParallelScene* scene);
    ParticlesEmiter(const ParticlesEmiter& copy);
    virtual ~ParticlesEmiter();

    ParticlesEmiter & operator=(const ParticlesEmiter& copy);

    void build();

    void render();

    void destroy();

    void setDeadEmiter(bool deadEmiter);
    bool isDeadEmiter() const;

    void setNumber(unsigned number);
    unsigned getNumber() const;

    void setTexture(Texture texture);
    Texture getTexture() const;

    void setDrawNumber(unsigned drawNumber);
    unsigned getDrawNumber() const;

    void setDepthTest(bool depthTest);
    bool isDepthTest() const;

    enum BlendEq
    {
        MODULAR,
        ADDITIVE,
    };

    void setBlendEq(BlendEq blendEq);
    BlendEq getBlendEq() const;

    ParticlesEmiter* clone();

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

    Particle* beginParticlesPosProcess();
    void endParticlesPosProcess();

    void setUsePointSprite(bool usePointSprite);
    bool isUsePointSprite() const;

    void setBulletSize(Vector2f bulletSize);
    Vector2f getBulletSize() const;

    static bool checkHardware();

protected:
    virtual void setupBullet(Particle& p);

protected:
    bool m_deadEmiter;
    bool m_depthTest;

    unsigned m_number;
    unsigned m_drawNumber;

    BlendEq m_blendEq;

    Texture m_texture;

    ParticlesParallelScene* m_parallelScene;

    ticks::Clock m_timestamp;

protected:
    float m_lifeInit;
    float m_lifeDown;
    float m_freeMove;

    bool m_continousMode;
    bool m_autoRebuild;

    Vector2f m_bulletSize;

    Vector3f m_emitPos;
    Vector3f m_gravity;
    Vector3f m_boxSize;

private:
    Particle::Array m_particles;
    GLuint m_renderId;
    bool m_usePointSprite;

    ParticlesEmiter& copy(const ParticlesEmiter& copy);
};

}
}

#endif
