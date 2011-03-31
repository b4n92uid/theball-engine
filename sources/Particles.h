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

    void Build();

    void Render();

    void Destroy();

    void SetDeadEmiter(bool deadEmiter);
    bool IsDeadEmiter() const;

    void SetNumber(unsigned number);
    unsigned GetNumber() const;

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

    static bool CheckHardware();

    CtorMap ConstructionMap(std::string root);

    Particle* BeginParticlesPosProcess();
    void EndParticlesPosProcess();

protected:
    virtual void SetupBullet(Particle& p) = 0;

protected:
    bool m_deadEmiter;
    bool m_depthTest;

    unsigned m_number;
    unsigned m_drawNumber;

    BlendEq m_blendEq;

    Texture m_texture;

    ParticlesParallelScene* m_parallelScene;

    ticks::Clock m_timestamp;

private:
    Particle::Array m_particles;

    GLuint m_renderId;
    GLuint m_auxRenderId;

    bool m_pointsprite;
};

}
}

#endif
