#ifndef _TBE_PARTICLES_H
#define _TBE_PARTICLES_H

#include <vector>

#include "Mathematics.h"
#include "Texture.h"

#include "Node.h"
#include "Material.h"

namespace tbe
{
namespace scene
{

/// \brief Représentation d'une particule

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

    ParticlesEmiter();
    ParticlesEmiter(const ParticlesEmiter& copy);
    virtual ~ParticlesEmiter();

    bool operator=(const ParticlesEmiter& copy);

    void Build();

    void Process();

    void Render();

    Node* Clone();

    void Destroy();

    void SetAutoRebuild(bool autoRebuild);
    bool IsAutoRebuild() const;

    void SetDeadEmiter(bool deadEmiter);
    bool IsDeadEmiter() const;

    void SetNumber(unsigned number);
    unsigned GetNumber() const;

    void SetFreeMove(float freeMove);
    float GetFreeMove() const;

    void SetContinousMode(bool continousMode);
    bool IsContinousMode() const;

    void SetLifeDown(float lifeDown);
    float GetLifeDown() const;

    void SetLifeInit(float lifeInit);
    float GetLifeInit() const;

    void SetGravity(Vector3f gravity);
    Vector3f GetGravity() const;

    void SetEndPos(Vector3f endPos);
    Vector3f GetEndPos() const;

    void SetTexture(Texture texture);
    Texture GetTexture() const;

    void SetDrawNumber(unsigned drawNumber);
    unsigned GetDrawNumber() const;

    void SetDepthTest(bool depthTest);
    bool IsDepthTest() const;

    enum BlendEq
    {
        MODULAR,
        ADDITIVE,
    };

    void SetBlendEq(BlendEq blendEq);
    BlendEq GetBlendEq() const;

    Particle* BeginParticlesPosProcess();
    void EndParticlesPosProcess();

    static void BeginSpiritDraw(float fovy, float viewportHeight);
    static void EndSpiritDraw();

    static bool CheckHardware();

protected:
    virtual void Build(Particle& p);

protected:
    float m_lifeInit;
    float m_lifeDown;
    float m_freeMove;

    bool m_continousMode;
    bool m_deadEmiter;
    bool m_autoRebuild;
    bool m_depthTest;

    Vector3f m_gravity;
    Vector3f m_endPos;

    unsigned m_number;
    unsigned m_drawNumber;

    BlendEq m_blendEq;

    Particle::Array m_particles;

    Texture m_texture;

    GLuint m_renderId;
};

}
}

#endif
