#include "Particles.h"
#include "Tools.h"

#include "SceneManager.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

// ParticlesEmiter -------------------------------------------------------------

ParticlesEmiter::ParticlesEmiter(ParticlesParallelScene* scene)
{
    m_pointsprite = CheckHardware();

    m_renderId = 0;

    glGenBuffersARB(1, &m_renderId);

    m_number = 0;
    m_drawNumber = 0;

    m_deadEmiter = false;
    m_enable = false;
    m_depthTest = false;

    m_blendEq = ADDITIVE;

    Node::m_parallelScene = m_parallelScene = scene;
    m_sceneManager = m_parallelScene->GetSceneManager();

    m_parallelScene->Register(this);
}

ParticlesEmiter::ParticlesEmiter(const ParticlesEmiter& copy) : Node(copy)
{
    m_pointsprite = CheckHardware();

    m_renderId = 0;

    glGenBuffersARB(1, &m_renderId);

    *this = copy;

    m_parallelScene->Register(this);
}

ParticlesEmiter::~ParticlesEmiter()
{
    Destroy();

    m_parallelScene->UnRegister(this);

    glDeleteBuffersARB(1, &m_renderId);
}

ParticlesEmiter& ParticlesEmiter::operator=(const ParticlesEmiter& copy)
{
    Node::operator=(copy);

    m_deadEmiter = copy.m_deadEmiter;

    m_depthTest = copy.m_depthTest;

    m_number = copy.m_number;

    m_drawNumber = copy.m_drawNumber;

    m_blendEq = copy.m_blendEq;
    m_texture = copy.m_texture;

    Node::m_parallelScene = m_parallelScene = copy.m_parallelScene;

    if(copy.m_enable)
        Build();

    return *this;
}

void ParticlesEmiter::Build()
{
    Destroy();

    for(unsigned i = 0; i < m_number; i++)
    {
        Particle p;

        SetupBullet(p);

        m_particles.push_back(p);
    }

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_renderId);

    if(m_pointsprite)
    {
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof (Particle) * m_number, &m_particles[0], GL_DYNAMIC_DRAW);
    }

    else
    {
        vector<Vertex> faces;
        faces.resize(m_number * 4);

        for(unsigned i = 0; i < faces.size(); i += 4)
        {
            faces[i + 0].texCoord(1, 1);
            faces[i + 1].texCoord(0, 1);
            faces[i + 2].texCoord(0, 0);
            faces[i + 3].texCoord(1, 0);
        }

        glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof (Vertex) * faces.size(), &faces[0], GL_DYNAMIC_DRAW);
    }

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

    m_enable = true;
    m_deadEmiter = false;

    m_timestamp.SnapShoot();
}

void ParticlesEmiter::Render()
{
    if(!m_enable || m_deadEmiter)
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

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_renderId);

    if(m_pointsprite)
    {
        glPushMatrix();
        glMultMatrixf(m_parent->GetAbsoluteMatrix());
        glMultMatrixf(m_matrix);
    }

    else
    {
        Matrix4f absolute = m_parent->GetAbsoluteMatrix();

        float size = m_parallelScene->GetParticleMinSize() / 2.0f;

        Vertex* auxParticles = static_cast<Vertex*>(glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB));

        for(unsigned i = 0; i < m_drawNumber * 4; i += 4)
        {
            Vector3f abspos = absolute * (m_matrix * m_particles[i / 4].pos);

            Matrix4f rot = m_sceneManager->computeBillboard(abspos);

            auxParticles[i + 0].pos = abspos + rot * Vector3f(size, -size, 0);
            auxParticles[i + 1].pos = abspos + rot * Vector3f(-size, -size, 0);
            auxParticles[i + 2].pos = abspos + rot * Vector3f(-size, size, 0);
            auxParticles[i + 3].pos = abspos + rot * Vector3f(size, size, 0);

            auxParticles[i + 0].color = m_particles[i / 4].color;
            auxParticles[i + 1].color = m_particles[i / 4].color;
            auxParticles[i + 2].color = m_particles[i / 4].color;
            auxParticles[i + 3].color = m_particles[i / 4].color;
        }

        glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
    }


    glDepthMask(m_depthTest);

    m_texture.Use();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    if(m_pointsprite)
    {
        const void* POSITION_OFFSET = (void*)0;
        const void* COLOR_OFFSET = (void*)(sizeof (Vector3f));

        glVertexPointer(3, GL_FLOAT, sizeof (Particle), POSITION_OFFSET);
        glColorPointer(4, GL_FLOAT, sizeof (Particle), COLOR_OFFSET);

        glDrawArrays(GL_POINTS, 0, m_drawNumber);
    }
    else
    {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        const void* POSITION_OFFSET = (void*)0;
        const void* COLOR_OFFSET = (void*)(sizeof (Vector3f) *2);
        const void* UV_OFFSET = (void*)(sizeof (Vector3f)*2 + sizeof (Vector4f));

        glVertexPointer(3, GL_FLOAT, sizeof (Vertex), POSITION_OFFSET);
        glColorPointer(4, GL_FLOAT, sizeof (Vertex), COLOR_OFFSET);
        glTexCoordPointer(2, GL_FLOAT, sizeof (Vertex), UV_OFFSET);

        glDrawArrays(GL_QUADS, 0, m_drawNumber * 4);
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);


    if(m_pointsprite)
        glPopMatrix();
    else
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

void ParticlesEmiter::Destroy()
{
    m_particles.clear();
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
    if(m_pointsprite)
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_renderId);
        return static_cast<Particle*>(glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_READ_WRITE_ARB));
    }
    else
        return &m_particles[0];
}

void ParticlesEmiter::EndParticlesPosProcess()
{
    if(m_pointsprite)
    {
        glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    }
}

bool ParticlesEmiter::CheckHardware()
{
    return GLEE_ARB_point_sprite && GLEE_ARB_point_parameters;
}

Node::CtorMap ParticlesEmiter::ConstructionMap(std::string root)
{
    Node::CtorMap ctormap = Node::ConstructionMap(root);

    ctormap["class"] = "ParticlesEmiter";

    ctormap["texture"] = tools::makeRelatifTo(root, m_texture.GetFilename());
    ctormap["number"] = tools::numToStr(m_number);

    return ctormap;
}
