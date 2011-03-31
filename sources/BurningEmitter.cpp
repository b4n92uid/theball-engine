/* 
 * File:   BurningEmitter.cpp
 * Author: b4n92uid
 * 
 * Created on 28 mars 2011, 18:50
 */

#include "BurningEmitter.h"

#include "Tools.h"

#include "SceneManager.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

BurningEmitter::BurningEmitter(ParticlesParallelScene* scene) : ParticlesEmiter(scene)
{
    m_lifeInit = 1;
    m_lifeDown = 0.1;
    m_freeMove = 0;

    m_autoRebuild = true;
    m_continousMode = false;
}

void BurningEmitter::SetBoxSize(Vector3f boxSize)
{
    this->m_boxSize = boxSize;
}

Vector3f BurningEmitter::GetBoxSize() const
{
    return m_boxSize;
}

BurningEmitter::BurningEmitter(const BurningEmitter& copy) : ParticlesEmiter(copy)
{
    *this = copy;
}

BurningEmitter::~BurningEmitter()
{
}

BurningEmitter& BurningEmitter::operator=(const BurningEmitter& copy)
{
    m_lifeInit = copy.m_lifeInit;
    m_lifeDown = copy.m_lifeDown;

    m_freeMove = copy.m_freeMove;

    m_continousMode = copy.m_continousMode;
    m_autoRebuild = copy.m_autoRebuild;

    m_gravity = copy.m_gravity;
    m_boxSize = copy.m_boxSize;

    return *this;
}

Node* BurningEmitter::Clone()
{
    return new BurningEmitter(*this);
}

void BurningEmitter::SetupBullet(Particle& p)
{
    p.life = 1;
    p.color = 1;
    p.diriction = tools::rand(Vector3f(-m_freeMove), Vector3f(m_freeMove));
    p.diriction.Normalize() *= m_freeMove;
    p.gravity = m_gravity;

    if(m_boxSize)
        p.pos = tools::rand(Vector3f(0), m_boxSize);
    else
        p.pos = 0;

    if(m_continousMode)
        p.life = tools::rand(0.0f, m_lifeInit);
    else
        p.life = m_lifeInit;
}

void BurningEmitter::Process()
{
    if(!m_enable)
        return;

    for_each(m_childs.begin(), m_childs.end(), mem_fun(&Node::Process));

    if(m_deadEmiter && !m_autoRebuild)
        return;

    long timestamp = m_timestamp.GetEsplanedTime();

    if(timestamp <= 0)
        return;

    Particle* particles = BeginParticlesPosProcess();

    m_deadEmiter = true;

    m_aabb.Clear();

    for(unsigned i = 0; i < m_drawNumber; i++)
    {
        Particle& p = particles[i];

        p.color(1, 1, 1, p.life);

        p.diriction += p.gravity;

        if(p.life < 0)
        {
            if(m_autoRebuild)
                SetupBullet(p);

            else
                continue;
        }

        else
        {

            p.life -= m_lifeDown * (1.0f / timestamp);
            p.pos += p.diriction * (1.0f / timestamp);

            m_deadEmiter = false;
        }

        m_aabb.Count(p.pos);
    }

    EndParticlesPosProcess();
}

void BurningEmitter::SetFreeMove(float freeMove)
{
    this->m_freeMove = freeMove;
}

float BurningEmitter::GetFreeMove() const
{
    return m_freeMove;
}

void BurningEmitter::SetContinousMode(bool continousMode)
{
    this->m_continousMode = continousMode;
}

bool BurningEmitter::IsContinousMode() const
{
    return m_continousMode;
}

void BurningEmitter::SetLifeDown(float lifeDown)
{
    this->m_lifeDown = lifeDown;
}

float BurningEmitter::GetLifeDown() const
{
    return m_lifeDown;
}

void BurningEmitter::SetGravity(Vector3f gravity)
{
    this->m_gravity = gravity;
}

Vector3f BurningEmitter::GetGravity() const
{
    return m_gravity;
}

void BurningEmitter::SetLifeInit(float lifeInit)
{
    this->m_lifeInit = lifeInit;
}

float BurningEmitter::GetLifeInit() const
{
    return m_lifeInit;
}

void BurningEmitter::SetAutoRebuild(bool autoRebuild)
{
    this->m_autoRebuild = autoRebuild;
}

bool BurningEmitter::IsAutoRebuild() const
{
    return m_autoRebuild;
}

Node::CtorMap BurningEmitter::ConstructionMap(std::string root)
{
    Node::CtorMap ctormap = ParticlesEmiter::ConstructionMap(root);

    ctormap["class"] = "BurningEmitter";

    ctormap["lifeInit"] = tools::numToStr(m_lifeInit);
    ctormap["lifeDown"] = tools::numToStr(m_lifeDown);
    ctormap["gravity"] = tools::numToStr(m_gravity);
    ctormap["freeMove"] = tools::numToStr(m_freeMove);
    ctormap["continousMode"] = tools::numToStr(m_continousMode);

    return ctormap;
}
