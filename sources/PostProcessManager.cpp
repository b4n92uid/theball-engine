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

PostProcessManager::PostProcessManager(const PostProcessManager& orig)
{
}

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

void PostProcessManager::setup(Vector2i screenSize)
{
    m_screenSize = tools::nextPow2(screenSize) / 2;

    if(!m_rttRender)
    {
        m_rttRender = new Rtt(m_screenSize);
        m_rttRender->setCaptureColor(true);
        m_rttRender->setCaptureDepth(true);
    }

    else
        m_rttRender->setFrameSize(m_screenSize);
}

Effect* PostProcessManager::getPostEffect(std::string name)
{
    if(m_postEffects.find(name) == m_postEffects.end())
        throw Exception("PostProcessManager::GetPostEffect; PostEffect not found (%s)", name.c_str());

    return m_postEffects[name];
}

Rtt* PostProcessManager::getRtt() const
{
    return m_rttRender;
}

void PostProcessManager::addPostEffect(std::string name, Effect* effect)
{
    if(name.empty())
        name = tools::nameGen(m_postEffects);

    else if(m_postEffects.find(name) != m_postEffects.end())
        throw Exception("PostProcessManager::AddPostEffect; PostEffect already exits (%s)", name.c_str());

    m_postEffects[name] = effect;
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
    glLoadIdentity();
}

void PostProcessManager::endPostProcess()
{
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

Effect::Effect()
{
    m_workRtt = new Rtt(128);
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

Layer::Layer()
{
    float vertexs[8] = {0, 0, 1, 0, 0, 1, 1, 1};

    glGenBuffers(1, &m_renderId);
    glBindBuffer(GL_ARRAY_BUFFER, m_renderId);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vertexs, GL_STATIC_DRAW);
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
    glTexCoordPointer(2, GL_FLOAT, 0, 0);
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
