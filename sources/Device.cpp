#include <iostream>
#include <time.h>

#include "Device.h"

#include "SceneManager.h"
#include "PostProcessManager.h"
#include "FPSManager.h"
#include "Eventrecv.h"
#include "GuiManager.h"
#include "Particles.h"
#include "Mesh.h"

using namespace std;
using namespace tbe;

Device::Device()
{
    #ifdef TBE_COMPILE_DEBUG
    cout << "Device::Device; Version (" << GetVersion() << ") Debug run" << endl;
    #else
    cout << "Device::Device; Version (" << GetVersion() << ") Release run" << endl;
    #endif

    m_eventManager = NULL;
    m_fpsManager = NULL;

    m_sceneManager = NULL;
    m_guiManager = NULL;
    m_postProcessManager = NULL;

    srand(time(0));
}

Device::~Device()
{
    if(m_eventManager) delete m_eventManager;
    if(m_fpsManager) delete m_fpsManager;
    if(m_sceneManager) delete m_sceneManager;
    if(m_guiManager) delete m_guiManager;
    if(m_postProcessManager) delete m_postProcessManager;
}

void Device::Setup(Vector2i winSize)
{
    this->m_winSize = winSize;

    if(!FrameBufferObject::CheckHardware())
        cout << "Device::Setup; Frame buffer object not supported" << endl;

    if(!Shader::CheckHardware())
        cout << "Device::Setup; Shaders not supported" << endl;

    if(!scene::Mesh::CheckHardware())
        cout << "Device::Setup; Vertex buffer object not supported" << endl;

    if(!scene::ParticlesEmiter::CheckHardware())
        cout << "Device::Setup; Point sprite particles not supported" << endl;


    if(m_eventManager) new(m_eventManager)EventManager;
    else m_eventManager = new EventManager;

    if(m_fpsManager) new(m_fpsManager)ticks::FpsManager;
    else m_fpsManager = new ticks::FpsManager;

    if(m_sceneManager) new(m_sceneManager)scene::SceneManager;
    else m_sceneManager = new scene::SceneManager;

    if(m_guiManager) new(m_guiManager)gui::GuiManager;
    else m_guiManager = new gui::GuiManager;

    if(m_postProcessManager) new(m_postProcessManager)ppe::PostProcessManager;
    else m_postProcessManager = new ppe::PostProcessManager;

    m_guiManager->Setup(winSize);
    m_sceneManager->Setup(winSize, 0, 70, 0.1, 512);
    m_postProcessManager->Setup(winSize);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Device::BeginScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Device::EndScene()
{
    glFlush();
}

Vector2i Device::GetWinSize() const
{
    return m_winSize;
}

scene::SceneManager* Device::GetSceneManager()
{
    return m_sceneManager;
}

EventManager* Device::GetEventManager()
{
    return m_eventManager;
}

gui::GuiManager* Device::GetGuiManager()
{
    return m_guiManager;
}

ticks::FpsManager* Device::GetFpsManager()
{
    return m_fpsManager;
}

ppe::PostProcessManager* Device::GetPostProcessManager() const
{
    return m_postProcessManager;
}

std::string Device::GetVersion()
{
    return __DATE__;
}

std::string Device::GetCWD()
{
    char cwd[MAX_PATH];
    getcwd(cwd, MAX_PATH);
    return cwd;
}
