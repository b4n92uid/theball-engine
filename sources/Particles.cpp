#include "Particles.h"
#include "Tools.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

// ParticlesEmiter -------------------------------------------------------------

ParticlesEmiter::ParticlesEmiter()
{
    glGenBuffers(1, &m_renderId);

    m_number = 0;

    m_lifeInit = 1;
    m_lifeDown = 0.1;
    m_freeMove = 0;

    m_autoRebuild = true;
    m_deadEmiter = false;
    m_continousMode = false;
    m_enable = false;
    m_depthTest = false;

    m_blendEq = ADDITIVE;
}

ParticlesEmiter::ParticlesEmiter(const ParticlesEmiter& copy)
{
    glGenBuffers(1, &m_renderId);

    *this = copy;
}

ParticlesEmiter::~ParticlesEmiter()
{
    glDeleteBuffers(1, &m_renderId);

    Destroy();
}

bool ParticlesEmiter::operator=(const ParticlesEmiter& copy)
{
    m_lifeInit = copy.m_lifeInit;
    m_lifeDown = copy.m_lifeDown;

    m_freeMove = copy.m_freeMove;

    m_continousMode = copy.m_continousMode;
    m_deadEmiter = copy.m_deadEmiter;
    m_autoRebuild = copy.m_autoRebuild;

    m_depthTest = copy.m_depthTest;

    m_gravity = copy.m_gravity;
    m_endPos = copy.m_endPos;

    m_number = copy.m_number;

    m_drawNumber = copy.m_drawNumber;

    m_blendEq = copy.m_blendEq;
    m_texture = copy.m_texture;

    if(copy.m_enable)
        Build();

    return true;
}

void ParticlesEmiter::Build(Particle& p)
{
    p.life = 1;
    p.color = 1;
    p.diriction = tools::rand(Vector3f(-m_freeMove), Vector3f(m_freeMove));
    p.diriction.Normalize() *= m_freeMove;
    p.gravity = m_gravity;

    if(m_endPos)
        p.pos = tools::rand(Vector3f(0), m_endPos);
    else
        p.pos = 0;

    if(m_continousMode)
        p.life = tools::rand(0.0f, m_lifeInit);
    else
        p.life = m_lifeInit;
}

void ParticlesEmiter::Build()
{
    Destroy();

    for(unsigned i = 0; i < m_number; i++)
    {
        Particle p;

        Build(p);

        m_particles.push_back(p);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_renderId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * m_number, &m_particles[0], GL_STREAM_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_enable = true;
}

Node* ParticlesEmiter::Clone()
{
    return new ParticlesEmiter(*this);
}

void ParticlesEmiter::Process()
{
    // Mise a jour du buffer

    Particle* particles = static_cast<Particle*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));

    m_deadEmiter = true;

    m_aabb.Clear();

    for(unsigned i = 0; i < m_particles.size(); i++)
    {
        Particle& p = particles[i];

        p.color(1, 1, 1, p.life);

        p.diriction += p.gravity;

        if(p.life < 0)
        {
            if(m_autoRebuild)
                Build(p);

            else
                continue;
        }

        else
        {
            p.life -= m_lifeDown;
            p.pos += p.diriction;

            m_deadEmiter = false;
        }

        m_aabb.Count(p.pos);
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void ParticlesEmiter::Render()
{
    if(!m_enable || (m_deadEmiter && !m_autoRebuild))
        return;

    switch(m_blendEq)
    {
        case ADDITIVE:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;
        case MODULAR:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_renderId);

    glDepthMask(m_depthTest);

    if(m_enableProcess)
        Process();

    glPushMatrix();

    #define POSITION_OFFSET (void*)0
    #define COLOR_OFFSET (void*)sizeof(Vector3f)

    m_texture.Use();

    glMultMatrixf(m_matrix);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, sizeof(Particle), POSITION_OFFSET);
    glColorPointer(4, GL_FLOAT, sizeof(Particle), COLOR_OFFSET);

    glDrawArrays(GL_POINTS, 0, m_drawNumber);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    #undef POSITION_OFFSET
    #undef COLOR_OFFSET

    glPopMatrix();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticlesEmiter::Destroy()
{
    m_particles.clear();
}

void ParticlesEmiter::SetEndPos(Vector3f endPos)
{
    this->m_endPos = endPos;
}

Vector3f ParticlesEmiter::GetEndPos() const
{
    return m_endPos;
}

void ParticlesEmiter::SetDepthTest(bool depthTest)
{
    m_depthTest = depthTest;
}

bool ParticlesEmiter::IsDepthTest() const
{
    return m_depthTest;
}

void ParticlesEmiter::SetDrawNumber(unsigned drawNumber)
{
    this->m_drawNumber = drawNumber;
}

unsigned ParticlesEmiter::GetDrawNumber() const
{
    return m_drawNumber;
}

void ParticlesEmiter::SetBlendEq(BlendEq blendEq)
{
    this->m_blendEq = blendEq;
}

ParticlesEmiter::BlendEq ParticlesEmiter::GetBlendEq() const
{
    return m_blendEq;
}

void ParticlesEmiter::SetLifeInit(float lifeInit)
{
    this->m_lifeInit = lifeInit;
}

float ParticlesEmiter::GetLifeInit() const
{
    return m_lifeInit;
}

void ParticlesEmiter::SetAutoRebuild(bool autoRebuild)
{
    this->m_autoRebuild = autoRebuild;
}

bool ParticlesEmiter::IsAutoRebuild() const
{
    return m_autoRebuild;
}

void ParticlesEmiter::SetDeadEmiter(bool deadEmiter)
{
    this->m_deadEmiter = deadEmiter;
}

bool ParticlesEmiter::IsDeadEmiter() const
{
    return m_deadEmiter;
}

void ParticlesEmiter::SetNumber(unsigned number)
{
    this->m_number = number;
    m_drawNumber = number;
}

unsigned ParticlesEmiter::GetNumber() const
{
    return m_number;
}

void ParticlesEmiter::SetFreeMove(float freeMove)
{
    this->m_freeMove = freeMove;
}

float ParticlesEmiter::GetFreeMove() const
{
    return m_freeMove;
}

void ParticlesEmiter::SetContinousMode(bool continousMode)
{
    this->m_continousMode = continousMode;
}

bool ParticlesEmiter::IsContinousMode() const
{
    return m_continousMode;
}

void ParticlesEmiter::SetLifeDown(float lifeDown)
{
    this->m_lifeDown = lifeDown;
}

float ParticlesEmiter::GetLifeDown() const
{
    return m_lifeDown;
}

void ParticlesEmiter::SetGravity(Vector3f gravity)
{
    this->m_gravity = gravity;
}

Vector3f ParticlesEmiter::GetGravity() const
{
    return m_gravity;
}

void ParticlesEmiter::SetTexture(Texture texture)
{
    this->m_texture = texture;
}

Texture ParticlesEmiter::GetTexture() const
{
    return m_texture;
}

Particle* ParticlesEmiter::BeginParticlesPosProcess()
{
    return static_cast<Particle*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
}

void ParticlesEmiter::EndParticlesPosProcess()
{
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void ParticlesEmiter::BeginSpiritDraw(float fovy, float viewportHeight)
{
    glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BITS | GL_POINT_BIT);

    glEnable(GL_POINT_SPRITE); // Active le remplacement du point par la texture
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glDisable(GL_LIGHTING);

    const float POINT_SIZE_CURRENT = 256;
    const float size = 128;

    float pixelPerUnit = viewportHeight / (2.0f * tan(fovy * 0.5f));
    float sqrtC = POINT_SIZE_CURRENT / (size * pixelPerUnit);

    Vector3f distAtt(0, 0, sqrtC * sqrtC);

    // Configuration de la taille des points suivant la formule :
    // derived_size = size * sqrt (1 / (a + b * d + c * d), d : distance en Z par rapport à l'écran.
    glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION_ARB, distAtt);

    // Activer le remplacement du poit par la texture
    glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);

    // Taille Maximal supporte par la Carte Graphique
    float maxSize = 0;
    glGetFloatv(GL_POINT_SIZE_MAX_ARB, &maxSize);

    // Limit Max/Min taille
    glPointParameterfARB(GL_POINT_SIZE_MIN_ARB, 1.0f);
    glPointParameterfARB(GL_POINT_SIZE_MAX_ARB, maxSize);

    // Fondue depuis le centre
    glPointParameterfARB(GL_POINT_FADE_THRESHOLD_SIZE_ARB, 60.0f);
}

void ParticlesEmiter::EndSpiritDraw()
{
    glPopAttrib();

}

bool ParticlesEmiter::CheckHardware()
{
    return GLEE_ARB_point_sprite;
}
