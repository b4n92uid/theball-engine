/* 
 * File:   RocketGuiManager.cpp
 * Author: b4n92uid
 * 
 * Created on 23 mars 2012, 15:09
 */

#include "RocketGuiManager.h"
#include "RocketGuiInterfaces.h"
#include <Tbe.h>

#include <boost\algorithm\algorithm.hpp>
#include <boost\filesystem.hpp>

#include <GL/gl.h>

using namespace std;
using namespace tbe;
using namespace tbe::gui;

namespace fs = boost::filesystem;

RocketGuiManager::RocketGuiManager()
{
    m_context = NULL;
    m_renderInterface = NULL;
    m_systemInterface = NULL;
    m_fileInterface = NULL;

}

RocketGuiManager::~RocketGuiManager()
{
    clearAll();

    m_context->RemoveReference();

    Rocket::Core::Shutdown();

    delete m_renderInterface;
    delete m_systemInterface;
    delete m_fileInterface;
}

void RocketGuiManager::setup(Vector2i viewport)
{
    m_renderInterface = new ShellRenderInterfaceOpenGL(viewport);
    Rocket::Core::SetRenderInterface(m_renderInterface);

    if(!m_systemInterface)
        m_systemInterface = new ShellSystemInterface;
    Rocket::Core::SetSystemInterface(m_systemInterface);

    m_fileInterface = new ShellFileInterface;
    Rocket::Core::SetFileInterface(m_fileInterface);

    Rocket::Core::Initialise();
    Rocket::Controls::Initialise();

    m_context = Rocket::Core::CreateContext("main", Rocket::Core::Vector2i(viewport.x, viewport.y));

    setViewport(viewport);
}

void RocketGuiManager::clearAll()
{
    m_context->UnloadAllDocuments();
}

void RocketGuiManager::render()
{
    // ----
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    glLoadIdentity();
    glOrtho(0, m_viewport.x, m_viewport.y, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // ----

    m_context->Render();

    // ----
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    // ----

    m_context->Update();
}

void RocketGuiManager::trasmitEvent(EventManager& e)
{
    int km;

    if(e.keyState[EventManager::KEY_LCTRL]) km |= Rocket::Core::Input::KM_CTRL;
    if(e.keyState[EventManager::KEY_LSHIFT]) km |= Rocket::Core::Input::KM_SHIFT;
    if(e.keyState[EventManager::KEY_LALT]) km |= Rocket::Core::Input::KM_ALT;
    if(e.keyState[EventManager::KEY_LMETA]) km |= Rocket::Core::Input::KM_META;
    if(e.keyState[EventManager::KEY_CAPSLOCK]) km |= Rocket::Core::Input::KM_CAPSLOCK;
    if(e.keyState[EventManager::KEY_NUMLOCK]) km |= Rocket::Core::Input::KM_NUMLOCK;
    if(e.keyState[EventManager::KEY_SCROLLOCK]) km |= Rocket::Core::Input::KM_SCROLLLOCK;

    if(e.notify == EventManager::EVENT_MOUSE_MOVE)
        m_context->ProcessMouseMove(e.mousePos.x, m_viewport.y - e.mousePos.y, km);

    else if(e.notify == EventManager::EVENT_MOUSE_DOWN)
    {
        if(e.lastActiveMouse.first == EventManager::MOUSE_BUTTON_WHEEL_UP)
            m_context->ProcessMouseWheel(-1, km);
        else if(e.lastActiveMouse.first == EventManager::MOUSE_BUTTON_WHEEL_DOWN)
            m_context->ProcessMouseWheel(+1, km);
        else
            m_context->ProcessMouseButtonDown(e.lastActiveMouse.first, km);
    }

    else if(e.notify == EventManager::EVENT_MOUSE_UP)
        m_context->ProcessMouseButtonUp(e.lastActiveMouse.first, km);

    else if(e.notify == EventManager::EVENT_KEY_DOWN)
    {
        m_context->ProcessKeyDown((Rocket::Core::Input::KeyIdentifier)e.lastActiveKey.first, km);
        m_context->ProcessTextInput(e.lastActiveKey.first);
    }

    else if(e.notify == EventManager::EVENT_KEY_UP)
        m_context->ProcessKeyUp((Rocket::Core::Input::KeyIdentifier)e.lastActiveKey.first, km);

    m_context->Update();
}

void RocketGuiManager::loadFonts(std::string dirpath)
{
	using namespace boost;
	
	vector<string> exts = { "ttf", "otf" };

	for (auto& entry : make_iterator_range(fs::directory_iterator(dirpath), {}))
        if(std::find(exts.begin(), exts.end(), entry.path().extension().string()) != exts.end())
            Rocket::Core::FontDatabase::LoadFontFace(entry.path().string().c_str());
}

void RocketGuiManager::addPath(std::string dirpath)
{
    m_fileInterface->includes.push_back(dirpath);
}

void RocketGuiManager::setViewport(Vector2i viewport)
{
    m_viewport = viewport;
    m_context->SetDimensions(Rocket::Core::Vector2i(viewport.x, viewport.y));
}

Vector2i RocketGuiManager::getViewport() const
{
    return m_viewport;
}

Rocket::Core::Context* RocketGuiManager::getContext() const
{
    return m_context;
}

