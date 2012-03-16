#include "Particles.h"
#include "Tools.h"

#include "SceneManager.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

// ParticlesEmiter -------------------------------------------------------------

ParticlesEmiter::ParticlesEmiter(ParticlesParallelScene* scene)
{
    m_usePointSprite = false;

    m_renderId = 0;

    glGenBuffersARB(1, &m_renderId);

    m_number = 0;
    m_drawNumber = 0;

    m_deadEmiter = false;
    m_enable = false;
    m_depthTest = false;

    m_blendEq = ADDITIVE;

    m_lifeInit = 1;
    m_lifeDown = 0.1;
    m_freeMove = 0;

    m_bulletSize = 0.5;

    m_autoRebuild = true;
    m_continousMode = false;

    Node::m_parallelScene = m_parallelScene = scene;
    m_sceneManager = m_parallelScene->getSceneManager();

    m_parallelScene->registerNode(this);
}

ParticlesEmiter::ParticlesEmiter(const ParticlesEmiter& copy) : Node(copy)
{
    m_renderId = 0;

    glGenBuffersARB(1, &m_renderId);

    this->copy(copy);

    m_parallelScene->registerNode(this);
}

ParticlesEmiter::~ParticlesEmiter()
{
    destroy();

    m_parallelScene->unregisterNode(this);

    glDeleteBuffersARB(1, &m_renderId);
}

ParticlesEmiter& ParticlesEmiter::operator=(const ParticlesEmiter& copy)
{
    Node::operator=(copy);

    return this->copy(copy);
}

ParticlesEmiter& ParticlesEmiter::copy(const ParticlesEmiter& copy)
{
    m_deadEmiter = copy.m_deadEmiter;
    m_depthTest = copy.m_depthTest;
    m_number = copy.m_number;
    m_drawNumber = copy.m_drawNumber;

    m_blendEq = copy.m_blendEq;

    m_texture = copy.m_texture;

    m_lifeInit = copy.m_lifeInit;
    m_lifeDown = copy.m_lifeDown;
    m_freeMove = copy.m_freeMove;
    m_bulletSize = copy.m_bulletSize;

    m_usePointSprite = copy.m_usePointSprite;

    m_continousMode = copy.m_continousMode;
    m_autoRebuild = copy.m_autoRebuild;

    m_emitPos = copy.m_emitPos;
    m_gravity = copy.m_gravity;
    m_boxSize = copy.m_boxSize;

    Node::m_parallelScene = m_parallelScene = copy.m_parallelScene;

    m_sceneManager = m_parallelScene->getSceneManager();

    if(copy.m_enable)
        build();

    return *this;
}

void ParticlesEmiter::build()
{
    destroy();

    for(unsigned i = 0; i < m_number; i++)
    {
        Particle p;

        setupBullet(p);

        m_particles.push_back(p);
    }

    m_drawNumber = m_number;

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_renderId);

    if(m_usePointSprite)
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

    m_timestamp.snapShoot();
}

void ParticlesEmiter::render()
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

    if(m_usePointSprite)
    {
        glPushMatrix();
        glMultMatrixf(m_parent->getAbsoluteMatrix());
        glMultMatrixf(m_matrix);
    }

    else
    {
        Matrix4 absolute = m_parent->getAbsoluteMatrix();

        Vertex* auxParticles = static_cast<Vertex*>(glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB));

        for(unsigned i = 0; i < m_drawNumber * 4; i += 4)
        {
            Vector3f abspos = absolute * (m_matrix * m_particles[i / 4].pos);

            Quaternion rot = m_sceneManager->computeBillboard(abspos).conjugate();

            auxParticles[i + 0].pos = abspos + rot * Vector3f(m_bulletSize.x, -m_bulletSize.y, 0);
            auxParticles[i + 1].pos = abspos + rot * Vector3f(-m_bulletSize.x, -m_bulletSize.y, 0);
            auxParticles[i + 2].pos = abspos + rot * Vector3f(-m_bulletSize.x, m_bulletSize.y, 0);
            auxParticles[i + 3].pos = abspos + rot * Vector3f(m_bulletSize.x, m_bulletSize.y, 0);

            auxParticles[i + 0].color = m_particles[i / 4].color;
            auxParticles[i + 1].color = m_particles[i / 4].color;
            auxParticles[i + 2].color = m_particles[i / 4].color;
            auxParticles[i + 3].color = m_particles[i / 4].color;
        }

        glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
    }


    glDepthMask(m_depthTest);

    m_texture.use();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    if(m_usePointSprite)
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


    if(m_usePointSprite)
        glPopMatrix();
    else
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

void ParticlesEmiter::destroy()
{
    m_particles.clear();
}

void ParticlesEmiter::setDepthTest(bool depthTest)
{
    m_depthTest = depthTest;
}

bool ParticlesEmiter::isDepthTest() const
{
    return m_depthTest;
}

void ParticlesEmiter::setDrawNumber(unsigned drawNumber)
{
    this->m_drawNumber = drawNumber;
}

unsigned ParticlesEmiter::getDrawNumber() const
{
    return m_drawNumber;
}

void ParticlesEmiter::setBlendEq(BlendEq blendEq)
{
    this->m_blendEq = blendEq;
}

ParticlesEmiter::BlendEq ParticlesEmiter::getBlendEq() const
{
    return m_blendEq;
}

void ParticlesEmiter::setDeadEmiter(bool deadEmiter)
{
    this->m_deadEmiter = deadEmiter;
}

bool ParticlesEmiter::isDeadEmiter() const
{
    return m_deadEmiter;
}

void ParticlesEmiter::setNumber(unsigned number)
{
    this->m_number = number;
}

unsigned ParticlesEmiter::getNumber() const
{
    return m_number;
}

void ParticlesEmiter::setTexture(Texture texture)
{
    this->m_texture = texture;
}

Texture ParticlesEmiter::getTexture() const
{
    return m_texture;
}

Particle* ParticlesEmiter::beginParticlesPosProcess()
{
    if(m_usePointSprite)
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_renderId);
        return static_cast<Particle*>(glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_READ_WRITE_ARB));
    }
    else
        return &m_particles[0];
}

void ParticlesEmiter::endParticlesPosProcess()
{
    if(m_usePointSprite)
    {
        glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    }
}

bool ParticlesEmiter::checkHardware()
{
    return GLEE_ARB_point_sprite && GLEE_ARB_point_parameters;
}

void ParticlesEmiter::setBulletSize(Vector2f bulletSize)
{
    this->m_bulletSize = bulletSize;
}

Vector2f ParticlesEmiter::getBulletSize() const
{
    return m_bulletSize;
}

void ParticlesEmiter::setUsePointSprite(bool usePointSprite)
{
    this->m_usePointSprite = usePointSprite && checkHardware();

    build();
}

bool ParticlesEmiter::isUsePointSprite() const
{
    return m_usePointSprite;
}

ParticlesEmiter* ParticlesEmiter::clone()
{
    return new ParticlesEmiter(*this);
}

void ParticlesEmiter::setupBullet(Particle& p)
{
    p.life = 1;
    p.color = 1;
    p.diriction = math::rand(Vector3f(-m_freeMove), Vector3f(m_freeMove));
    p.diriction.normalize() *= m_freeMove;
    p.gravity = m_gravity;

    if(!!m_boxSize)
        p.pos = math::rand(Vector3f(0), m_boxSize);
    else
        p.pos = m_emitPos;

    if(m_continousMode)
        p.life = math::rand(0.0f, m_lifeInit);
    else
        p.life = m_lifeInit;
}

void ParticlesEmiter::process()
{
    if(!m_enable)
        return;

    for_each(m_childs.begin(), m_childs.end(), mem_fun(&Node::process));

    if(m_deadEmiter && !m_autoRebuild)
        return;

    m_deadEmiter = true;

    long timestamp = m_timestamp.getEsplanedTime();

    Particle* particles = beginParticlesPosProcess();

    m_aabb.clear();

    for(unsigned i = 0; i < m_drawNumber; i++)
    {
        Particle& p = particles[i];

        p.color(1, 1, 1, p.life);

        p.diriction += p.gravity;

        if(p.life < 0)
        {
            if(m_autoRebuild)
                setupBullet(p);

            else
                continue;
        }

        else
        {
            p.life -= m_lifeDown * (timestamp * 0.001f);
            p.pos += p.diriction * (timestamp * 0.001f);

            m_deadEmiter = false;
        }

        m_aabb.count(p.pos);
    }

    endParticlesPosProcess();
}

void ParticlesEmiter::setFreeMove(float freeMove)
{
    this->m_freeMove = freeMove;
}

float ParticlesEmiter::getFreeMove() const
{
    return m_freeMove;
}

void ParticlesEmiter::setContinousMode(bool continousMode)
{
    this->m_continousMode = continousMode;
}

bool ParticlesEmiter::isContinousMode() const
{
    return m_continousMode;
}

void ParticlesEmiter::setLifeDown(float lifeDown)
{
    this->m_lifeDown = lifeDown;
}

float ParticlesEmiter::getLifeDown() const
{
    return m_lifeDown;
}

void ParticlesEmiter::setGravity(Vector3f gravity)
{
    this->m_gravity = gravity;
}

Vector3f ParticlesEmiter::getGravity() const
{
    return m_gravity;
}

void ParticlesEmiter::setLifeInit(float lifeInit)
{
    this->m_lifeInit = lifeInit;
}

float ParticlesEmiter::getLifeInit() const
{
    return m_lifeInit;
}

void ParticlesEmiter::setEmitPos(Vector3f emitPos)
{
    this->m_emitPos = emitPos;
}

Vector3f ParticlesEmiter::getEmitPos() const
{
    return m_emitPos;
}

void ParticlesEmiter::setAutoRebuild(bool autoRebuild)
{
    this->m_autoRebuild = autoRebuild;
}

bool ParticlesEmiter::isAutoRebuild() const
{
    return m_autoRebuild;
}

void ParticlesEmiter::setBoxSize(Vector3f boxSize)
{
    this->m_boxSize = boxSize;
}

Vector3f ParticlesEmiter::getBoxSize() const
{
    return m_boxSize;
}

Node::CtorMap ParticlesEmiter::constructionMap(std::string root)
{
    Node::CtorMap ctormap = Node::constructionMap(root);

    ctormap["class"] = "ParticlesEmiter";

    ctormap["texture"] = tools::pathScope(root, m_texture.getFilename(), false);
    ctormap["number"] = tools::numToStr(m_number);
    ctormap["lifeInit"] = tools::numToStr(m_lifeInit);
    ctormap["lifeDown"] = tools::numToStr(m_lifeDown);
    ctormap["gravity"] = tools::numToStr(m_gravity);
    ctormap["boxSize"] = tools::numToStr(m_boxSize);
    ctormap["bulletSize"] = tools::numToStr(m_bulletSize);
    ctormap["freeMove"] = tools::numToStr(m_freeMove);
    ctormap["continousMode"] = tools::numToStr(m_continousMode);
    ctormap["usePointSprite"] = tools::numToStr(m_usePointSprite);

    return ctormap;
}

std::vector<std::string> ParticlesEmiter::getUsedRessources()
{
    std::vector<std::string> paths;
    paths.push_back(m_texture.getFilename());

    return paths;
}
