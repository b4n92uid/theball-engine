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
    cout << endl;
    cout << "Closing...";

    if(m_eventManager) delete m_eventManager;
    if(m_fpsManager) delete m_fpsManager;
    if(m_sceneManager) delete m_sceneManager;
    if(m_guiManager) delete m_guiManager;
    if(m_postProcessManager) delete m_postProcessManager;

    Texture::resetCache();

    cout << "OK" << endl;
    cout << endl;
}

void Device::init()
{
    const char * shaderver = Shader::checkHardware() ?
            (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION) : "N/A";
    cout

            #ifdef TBE_COMPILE_DEBUG
            << "theBall engine (" << getVersion() << ") Debug run" << endl
            #else
            << "theBall engine (" << getVersion() << ") Release run" << endl
            #endif
            << "Vendor: " << glGetString(GL_VENDOR) << endl
            << "Render: " << glGetString(GL_RENDERER) << endl
            << "OpenGL: " << glGetString(GL_VERSION) << endl
            << "Shader: " << shaderver << endl
            << endl;

    cout << "Initing..." << endl;

    if(!FrameBufferObject::checkHardware())
        cout << "*** WARNING *** Device::Setup; Frame buffer object not supported" << endl;

    if(!Shader::checkHardware())
        cout << "*** WARNING *** Device::Setup; Shaders not supported" << endl;

    if(!HardwareBuffer::checkHardware())
        cout << "*** WARNING *** Device::Setup; Vertex buffer object not supported" << endl;

    if(!scene::ParticlesEmiter::checkHardware())
        cout << "*** WARNING *** Device::Setup; Point sprite particles not supported" << endl;

    cout << endl;

    cout << "Init event manager" << endl;

    if(m_eventManager)
        new(m_eventManager)EventManager;
    else
        m_eventManager = new EventManager;

    cout << "Init framerate manager" << endl;

    if(m_fpsManager)
        new(m_fpsManager)ticks::FpsManager;
    else
        m_fpsManager = new ticks::FpsManager;

    cout << "Init scene manager" << endl;

    if(m_sceneManager)
        new(m_sceneManager)scene::SceneManager;
    else
        m_sceneManager = new scene::SceneManager;

    cout << "Init gui manager" << endl;

    if(m_guiManager)
        new(m_guiManager)gui::GuiManager;
    else
        m_guiManager = new gui::GuiManager;

    cout << "Init post process effets manager" << endl;

    if(m_postProcessManager)
        new(m_postProcessManager)ppe::PostProcessManager;
    else
        m_postProcessManager = new ppe::PostProcessManager;

    cout << "Init OpenGL state" << endl;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    cout << endl;
}

void Device::setViewportSize(Vector2i viewportSize)
{
    m_viewportSize = viewportSize;

    m_guiManager->setup(m_viewportSize);
    m_sceneManager->setup(m_viewportSize, 0, 70, 0.1, 512);
    m_postProcessManager->setup(m_viewportSize);
}

Vector2i Device::getViewportSize() const
{
    return m_viewportSize;
}

void Device::beginScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Device::endScene()
{
    glFlush();
}

scene::SceneManager* Device::getSceneManager()
{
    return m_sceneManager;
}

EventManager* Device::getEventManager()
{
    return m_eventManager;
}

gui::GuiManager* Device::getGuiManager()
{
    return m_guiManager;
}

ticks::FpsManager* Device::getFpsManager()
{
    return m_fpsManager;
}

ppe::PostProcessManager* Device::getPostProcessManager() const
{
    return m_postProcessManager;
}

std::string Device::getVersion()
{
    return __DATE__;
}
