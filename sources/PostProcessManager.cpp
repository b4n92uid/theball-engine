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
    ClearAll();

    delete m_rttRender;
}

void PostProcessManager::ClearAll()
{
    for(Effect::Map::iterator itt = m_postEffects.begin(); itt != m_postEffects.end(); itt++)
        delete itt->second;

    m_postEffects.clear();
}

void PostProcessManager::Setup(Vector2i screenSize)
{
    m_screenSize = tools::next_p2(screenSize) / 2;
    //    m_screenSize = screenSize;

    if(!m_rttRender)
    {
        m_rttRender = new Rtt(m_screenSize);
        m_rttRender->SetCaptureColor(true);
        m_rttRender->SetCaptureDepth(true);
    }

    else
        m_rttRender->SetFrameSize(m_screenSize);
}

Effect* PostProcessManager::GetPostEffect(std::string name)
{
    if(m_postEffects.find(name) == m_postEffects.end())
        throw Exception("PostProcessManager::GetPostEffect; PostEffect not found (%s)", name.c_str());

    return m_postEffects[name];
}

Rtt* PostProcessManager::GetRtt() const
{
    return m_rttRender;
}

void PostProcessManager::AddPostEffect(std::string name, Effect* effect)
{
    if(name.empty())
        name = tools::NameGen(m_postEffects);

    else if(m_postEffects.find(name) != m_postEffects.end())
        throw Exception("PostProcessManager::AddPostEffect; PostEffect already exits (%s)", name.c_str());

    m_postEffects[name] = effect;
}

void PostProcessManager::BeginPostProcess()
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

void PostProcessManager::EndPostProcess()
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);

    glPopAttrib();
}

void PostProcessManager::Render()
{
    /*
     * Etape des effets apres traitement :
     * -----------------------------------
     * Passage en 2D
     * Pour chaque effet : traitement sur fbo
     * Rendue fbo -> écran
     */

    BeginPostProcess();

    bool usingRB = FrameBufferObject::CheckHardware()
            && m_rttRender->FBO_IsUseRenderBuffer();

    if(usingRB)
    {
        m_rttRender->FBO_BlitToTexutre();
        m_rttRender->FBO_SetUseRenderBuffer(false);
    }

    for(Effect::Map::iterator itt = m_postEffects.begin(); itt != m_postEffects.end(); itt++)
        if(itt->second->IsEnable())
            itt->second->Process(m_rttRender);

    m_rttRender->GetColor().Use(true);
    m_layer.Draw();

    if(usingRB)
    {
        m_rttRender->FBO_SetUseRenderBuffer(true);
    }

    EndPostProcess();
}

// Effect ----------------------------------------------------------------------

Effect::Effect()
{
    m_workRtt = new Rtt(127);
    m_workRtt->SetCaptureColor(true);
    m_workRtt->SetCaptureDepth(true);

    m_enable = true;
}

Effect::~Effect()
{
    delete m_workRtt;
}

void Effect::SetRttFrameSize(Vector2i size)
{
    m_workRtt->SetFrameSize(size);
}

Vector2i Effect::GetRttFrameSize()
{
    return m_workRtt->GetFrameSize();
}

void Effect::SetEnable(bool enable)
{
    this->m_enable = enable;
}

bool Effect::IsEnable() const
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

void Layer::Begin()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_renderId);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, 0);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);
}

void Layer::Draw(bool autoSetup)
{
    if(autoSetup)
        Begin();

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    if(autoSetup)
        End();
}

void Layer::End()
{
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
