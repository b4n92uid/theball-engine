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

void Device::Init()
{
#ifdef TBE_COMPILE_DEBUG
    cout << "theBall engine (" << GetVersion() << ") Debug run" << endl;
#else
    cout << "theBall engine (" << GetVersion() << ") Release run" << endl;
#endif

    cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
    cout << "Render: " << glGetString(GL_RENDERER) << endl;
    cout << "OpenGL: " << glGetString(GL_VERSION) << endl;
    cout << "Shader: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    if(!FrameBufferObject::CheckHardware())
        cout << "Device::Setup; Frame buffer object not supported" << endl;

    if(!Shader::CheckHardware())
        cout << "Device::Setup; Shaders not supported" << endl;

    if(!scene::Mesh::CheckHardware())
        cout << "Device::Setup; Vertex buffer object not supported" << endl;

    if(!scene::ParticlesEmiter::CheckHardware())
        cout << "Device::Setup; Point sprite particles not supported" << endl;

    if(m_eventManager)
        new(m_eventManager) EventManager;
    else
        m_eventManager = new EventManager;

    if(m_fpsManager)
        new(m_fpsManager) ticks::FpsManager;
    else
        m_fpsManager = new ticks::FpsManager;

    if(m_sceneManager)
        new(m_sceneManager) scene::SceneManager;
    else
        m_sceneManager = new scene::SceneManager;

    if(m_guiManager)
        new(m_guiManager) gui::GuiManager;
    else
        m_guiManager = new gui::GuiManager;

    if(m_postProcessManager)
        new(m_postProcessManager) ppe::PostProcessManager;
    else
        m_postProcessManager = new ppe::PostProcessManager;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Device::SetViewportSize(Vector2i viewportSize)
{
    m_viewportSize = viewportSize;

    m_guiManager->Setup(m_viewportSize);
    m_sceneManager->Setup(m_viewportSize, 0, 70, 0.1, 512);
    m_postProcessManager->Setup(m_viewportSize);
}

Vector2i Device::GetViewportSize() const
{
    return m_viewportSize;
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
