/* 
 * File:   CEGUIManager.cpp
 * Author: b4n92uid
 * 
 * Created on 22 mars 2012, 15:15
 */

#include "CEGUIManager.h"

#include "Eventrecv.h"
#include "Tools.h"
#include "Device.h"

using namespace tbe;
using namespace gui;
using namespace CEGUI;

CEGUIManager::CEGUIManager()
{
}

CEGUIManager::~CEGUIManager()
{
}

void CEGUIManager::setup(Vector2i scrsize)
{
    m_renderer = &CEGUI::OpenGLRenderer::bootstrapSystem();

    m_system = CEGUI::System::getSingletonPtr();

    m_defaultResProvider = static_cast<CEGUI::DefaultResourceProvider*>(m_system->getResourceProvider());

    m_winManager = CEGUI::WindowManager::getSingletonPtr();

    m_root = m_winManager->createWindow("DefaultWindow", "_MasterRoot");
    m_system->setGUISheet(m_root);

    setViewport(scrsize);
}

void CEGUIManager::render()
{
    m_system->renderGUI();
}

void CEGUIManager::clearAll()
{
}

CEGUI::MouseButton translateMouseButton(int mc)
{
    switch(mc)
    {
        case EventManager::MOUSE_BUTTON_LEFT: return CEGUI::LeftButton;
        case EventManager::MOUSE_BUTTON_RIGHT: return CEGUI::RightButton;
        case EventManager::MOUSE_BUTTON_MIDDLE: return CEGUI::MiddleButton;
    }
}

void CEGUIManager::trasmitEvent(EventManager& e)
{
    if(e.notify == EventManager::EVENT_MOUSE_MOVE)
        m_system->injectMousePosition(e.mousePos.x, m_viewport.y - e.mousePos.y);

    if(e.notify == EventManager::EVENT_MOUSE_DOWN)
        m_system->injectMouseButtonDown(translateMouseButton(e.lastActiveMouse.first));

    if(e.notify == EventManager::EVENT_MOUSE_UP)
        m_system->injectMouseButtonUp(translateMouseButton(e.lastActiveMouse.first));

    if(e.notify == EventManager::EVENT_KEY_DOWN)
        m_system->injectKeyDown(e.lastScanCode);

    if(e.notify == EventManager::EVENT_KEY_UP)
        m_system->injectKeyUp(e.lastScanCode);

    m_system->injectTimePulse(e.lastPollTimestamp);
}

void CEGUIManager::setScheme(std::string id)
{
    CEGUI::SchemeManager::getSingleton().create(id);
}

void CEGUIManager::setRessources(std::string path)
{
    m_defaultResProvider->setResourceGroupDirectory("imagesets", path);
    m_defaultResProvider->setResourceGroupDirectory("fonts", path);
    m_defaultResProvider->setResourceGroupDirectory("schemes", path);
    m_defaultResProvider->setResourceGroupDirectory("layouts", path);
    m_defaultResProvider->setResourceGroupDirectory("looknfeels", path);
    m_defaultResProvider->setResourceGroupDirectory("lua_scripts", path);
    m_defaultResProvider->setResourceGroupDirectory("animations", path);

    // Sets the default resource groups to be used:
    CEGUI::Imageset::setDefaultResourceGroup("imagesets");
    CEGUI::Font::setDefaultResourceGroup("fonts");
    CEGUI::Scheme::setDefaultResourceGroup("schemes");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
    CEGUI::WindowManager::setDefaultResourceGroup("layouts");
    CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
    CEGUI::AnimationManager::setDefaultResourceGroup("animations");
}

void CEGUIManager::setViewport(Vector2i viewport)
{
    m_viewport = viewport;
    // m_system->getRenderer()->setDisplaySize(CEGUI::Size(viewport.x, viewport.y));
}

Vector2i CEGUIManager::getViewport() const
{
    return m_viewport;
}

CEGUI::Window* CEGUIManager::getRoot() const
{
    return m_root;
}

CEGUI::WindowManager* CEGUIManager::getWindowManager() const
{
    return m_winManager;
}

CEGUI::System* CEGUIManager::getSystem() const
{
    return m_system;
}
