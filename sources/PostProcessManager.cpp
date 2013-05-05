/*
 * File:   PostProcessManager.cpp
 * Author: b4n92uid
 *
 * Created on 4 janvier 2010, 14:39
 */

#include "PostProcessManager.h"

#include "Tools.h"
#include "Exception.h"

#include <map>

using namespace std;

using namespace tbe;
using namespace tbe::ppe;


// PostProcessManager ----------------------------------------------------------

PostProcessManager::PostProcessManager()
{
    m_rttRender = NULL;
}

PostProcessManager::PostProcessManager(const PostProcessManager& orig) { }

PostProcessManager::~PostProcessManager()
{
    clearAll();

    delete m_rttRender;
}

void PostProcessManager::clearAll()
{
    for(Effect::Map::iterator itt = m_postEffects.begin(); itt != m_postEffects.end(); itt++)
        delete itt->second;

    m_postEffects.clear();
}

void PostProcessManager::setup(Vector2i viewport)
{
    setViewport(viewport);
}

Effect* PostProcessManager::getPostEffect(std::string name)
{
    if(m_postEffects.find(name) == m_postEffects.end())
        throw Exception("PostProcessManager::getPostEffect; PostEffect not found (%s)", name.c_str());

    return m_postEffects[name];
}

Rtt* PostProcessManager::getRtt() const
{
    return m_rttRender;
}

void PostProcessManager::setViewport(Vector2i viewport)
{
    this->m_viewport = viewport;

    m_viewport = math::nextPow2(m_viewport) / 2;

    if(!m_rttRender)
    {
        m_rttRender = new Rtt(m_viewport);
        m_rttRender->setCaptureColor(true);
        m_rttRender->setCaptureDepth(true);
    }

    else
        m_rttRender->setFrameSize(m_viewport);
}

Vector2i PostProcessManager::getViewport() const
{
    return m_viewport;
}

void PostProcessManager::addPostEffect(std::string name, Effect* effect)
{
    if(name.empty())
        name = tools::nameGen(m_postEffects);

    else if(m_postEffects.find(name) != m_postEffects.end())
        throw Exception("PostProcessManager::AddPostEffect; PostEffect already exits (%s)", name.c_str());

    m_postEffects[name] = effect;
}

Effect* PostProcessManager::releasePostEffect(std::string name)
{
    if(!m_postEffects.count(name))
        throw Exception("PostProcessManager::releasePostEffect; PostEffect not found (%s)", name.c_str());

    Effect* e = m_postEffects[name];

    m_postEffects.erase(name);

    return e;
}

void PostProcessManager::deletePostEffect(std::string name)
{
    if(!m_postEffects.count(name))
        throw Exception("PostProcessManager::releasePostEffect; PostEffect not found (%s)", name.c_str());

    delete m_postEffects[name];

    m_postEffects.erase(name);
}

void PostProcessManager::deletePostEffect(Effect* effect)
{
    for(Effect::Map::iterator it = m_postEffects.begin(); it != m_postEffects.end(); it++)
    {
        if(it->second == effect)
        {
            m_postEffects.erase(it);
            delete effect;
            return;
        }
    }
}

void PostProcessManager::beginPostProcess()
{
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);

    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

void PostProcessManager::endPostProcess()
{
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);

    glPopAttrib();
}

void PostProcessManager::render()
{
    /*
     * Etape des effets apres traitement :
     * -----------------------------------
     * Passage en 2D
     * Pour chaque effet : traitement sur fbo
     * Rendue fbo -> écran
     */

    beginPostProcess();

    bool usingRB = FrameBufferObject::checkHardware()
            && m_rttRender->FBO_IsUseRenderBuffer();

    if(usingRB)
    {
        m_rttRender->FBO_BlitToTexutre();
        m_rttRender->FBO_SetUseRenderBuffer(false);
    }

    for(Effect::Map::iterator itt = m_postEffects.begin(); itt != m_postEffects.end(); itt++)
        if(itt->second->isEnable())
            itt->second->process(m_rttRender);

    m_rttRender->getColor().use(true);
    m_layer.draw();

    if(usingRB)
    {
        m_rttRender->FBO_SetUseRenderBuffer(true);
    }

    endPostProcess();
}

// Effect ----------------------------------------------------------------------

Vector2i Effect::defaultSize = 128;

Effect::Effect()
{
    m_workRtt = new Rtt(Effect::defaultSize);
    m_workRtt->setCaptureColor(true);
    m_workRtt->setCaptureDepth(true);

    m_enable = true;
}

Effect::~Effect()
{
    delete m_workRtt;
}

void Effect::setRttFrameSize(Vector2i size)
{
    m_workRtt->setFrameSize(size);
}

Vector2i Effect::getRttFrameSize()
{
    return m_workRtt->getFrameSize();
}

void Effect::setEnable(bool enable)
{
    this->m_enable = enable;
}

bool Effect::isEnable() const
{
    return m_enable;
}

// Layer -----------------------------------------------------------------------

Layer::Layer(Vector2f pos, Vector2f size)
{
    //    float vertexs[8] = {0, 0, 1, 0, 0, 1, 1, 1};
    Vector2f pos2 = pos + size;
    float vertexs[16] = {
        pos.x, pos.y, pos2.x, pos.y, pos.x, pos2.y, pos2.x, pos2.y,
        0, 0, 1, 0, 0, 1, 1, 1
    };

    glGenBuffers(1, &m_renderId);
    glBindBuffer(GL_ARRAY_BUFFER, m_renderId);
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof (float), vertexs, GL_STATIC_DRAW);
}

Layer::~Layer()
{
    glDeleteBuffers(1, &m_renderId);
}

void Layer::begin()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_renderId);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, 0);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*) (8 * sizeof (float)));
}

void Layer::draw(bool autoSetup)
{
    if(autoSetup)
        begin();

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    if(autoSetup)
        end();
}

void Layer::end()
{
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
