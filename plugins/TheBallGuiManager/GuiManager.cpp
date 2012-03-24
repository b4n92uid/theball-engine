#include "GuiManager.h"

#include "Eventrecv.h"
#include "FPSManager.h"
#include "Tools.h"
#include "Device.h"

#include "KeyConfig.h"
#include "Button.h"
#include "Gauge.h"
#include "TextBox.h"
#include "Editbox.h"
#include "Switch.h"
#include "Image.h"
#include "StateShow.h"
#include "Layout.h"
#include "Skin.h"

using namespace tbe;
using namespace tbe::gui;
using namespace std;

// GuiManager ------------------------------------------------------------------

GuiManager::GuiManager()
{
    m_guiSkin = NULL;
    m_currentSession = m_sessions.end();

    setSession(0);
}

GuiManager::~GuiManager()
{
    clearAll();

    if(m_guiSkin)
        delete m_guiSkin;
}

void GuiManager::clearAll()
{
    for(GuiManager::Session::Map::iterator its = m_sessions.begin(); its != m_sessions.end(); its++)
        delete its->second;

    m_sessions.clear();
    m_currentSession = m_sessions.end();
}

void GuiManager::setup(Vector2i viewport)
{
    setViewport(viewport);
}

void GuiManager::updateLayout()
{
    for_each(m_currentSession->second->headLayouts.begin(),
             m_currentSession->second->headLayouts.end(),
             mem_fun(&Layout::update));
}

void GuiManager::render()
{
    // Passage en Ortho 2D
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    glLoadIdentity();
    gluOrtho2D(0, m_viewport.x, 0, m_viewport.y);

    // --
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_TEXTURE_2D);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    updateLayout();

    for(Control::Map::iterator itt = m_currentSession->second->ctrls.begin();
        itt != m_currentSession->second->ctrls.end(); itt++)
        itt->second->render();

    glPopAttrib();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void GuiManager::pageUp()
{
    m_currentSession--;

    if(m_currentSession == --m_sessions.begin())
        m_currentSession = --m_sessions.end();
}

void GuiManager::pageDown()
{
    m_currentSession++;

    if(m_currentSession == m_sessions.end())
        m_currentSession = m_sessions.begin();
}

void GuiManager::setSession(unsigned sessID, GuiSkin* skin)
{
    if(m_currentSession->first != sessID && m_currentSession != m_sessions.end())
        for(Control::Map::iterator itt = m_currentSession->second->ctrls.begin();
            itt != m_currentSession->second->ctrls.end(); itt++)
            itt->second->setActivate(false);

    if(m_sessions.find(sessID) == m_sessions.end())
        m_sessions[sessID] = new Session;

    m_sessions[sessID]->guiSkin = skin;

    m_currentSession = m_sessions.find(sessID);
}

unsigned GuiManager::getSession()
{
    return m_currentSession->first;
}

void GuiManager::destroySession(unsigned sessID)
{
    if(m_currentSession->first == sessID)
        m_currentSession++;

    delete m_sessions[sessID];
    m_sessions.erase(sessID);

    if(m_currentSession == m_sessions.end())
        if(m_sessions.size() > 0)
            m_currentSession = m_sessions.begin();
}

void GuiManager::trasmitEvent(EventManager& e)
{
    if(m_currentSession != m_sessions.end())
        for(Control::Map::iterator it = m_currentSession->second->ctrls.begin();
            it != m_currentSession->second->ctrls.end(); ++it)
            if(it->second->onEvent(e))
            {
                e.notify = EventManager::EVENT_NO_EVENT;
                break;
            }
}

void GuiManager::setSkin(GuiSkin* skin)
{
    if(m_guiSkin)
        delete m_guiSkin;

    m_guiSkin = skin;

    if(m_guiSkin)
        for(Session::Map::iterator i = m_sessions.begin(); i != m_sessions.end(); i++)
            for(Control::Map::iterator j = i->second->ctrls.begin(); j != i->second->ctrls.end(); j++)
                j->second->setSkin(*m_guiSkin);

}

TextBox* GuiManager::addTextBox(std::string name)
{
    TextBox* p = new TextBox;

    addControl(name, p);

    return p;
}

KeyConfig* GuiManager::addKeyConfig(std::string name)
{
    KeyConfig* p = new KeyConfig;

    addControl(name, p);

    return p;
}

Gauge* GuiManager::addGauge(std::string name, std::string label)
{
    Gauge* p = new Gauge;
    p->setLabel(label);

    addControl(name, p);

    return p;
}

EditBox* GuiManager::addEditBox(std::string name, std::string label)
{
    EditBox* p = new EditBox;
    p->setLabel(label);

    addControl(name, p);

    return p;
}

SwitchString* GuiManager::addSwitchString(std::string name)
{
    SwitchString* p = new SwitchString;

    addControl(name, p);

    return p;
}

Image* GuiManager::addImage(std::string name, Texture path)
{

    Image* p = new Image(path);

    addControl(name, p);

    return p;
}

StateShow* GuiManager::addStateShow(std::string name, Texture path, int metaCount)
{

    StateShow* p = new StateShow;
    p->setBackground(path);
    p->setMetaCount(metaCount);

    addControl(name, p);

    return p;
}

Button* GuiManager::addButton(std::string name, std::string label)
{
    Button* p = new Button;
    p->setLabel(label);

    addControl(name, p);

    return p;
}

void GuiManager::addControl(std::string name, Control* ctrl)
{
    if(name.empty())
        name = tools::nameGen(m_currentSession->second->ctrls);

    if(m_currentSession->second->ctrls.find(name) != m_currentSession->second->ctrls.end())
        throw Exception("GuiManager::AddControl; Control already exist (%s)", name.c_str());

    m_currentSession->second->ctrls[name] = ctrl;

    if(m_currentSession->second->guiSkin)
        ctrl->setSkin(*m_currentSession->second->guiSkin);

    else if(m_guiSkin)
        ctrl->setSkin(*m_guiSkin);

    ctrl->setName(name);

    if(m_currentSession->second->activeLayout)
        m_currentSession->second->activeLayout->addControl(ctrl);
}

void GuiManager::deleteControl(std::string name)
{

    if(m_currentSession->second->ctrls.find(name) == m_currentSession->second->ctrls.end())
        throw Exception("GuiManager::DeleteControl; Control not found (%s); Session #%d", name.c_str(), m_currentSession->first);

    delete m_currentSession->second->ctrls[name];
    m_currentSession->second->ctrls.erase(name);
}

void GuiManager::deleteControls(Control* ctrl)
{
    for(Session::Map::iterator it = m_sessions.begin(); it != m_sessions.end(); it++)
    {
        for(Control::Map::iterator it2 = it->second->ctrls.begin(); it2 != it->second->ctrls.end(); it2++)
            if(it2->second == ctrl)
                it->second->ctrls.erase(it2->first);
    }
}

Control* GuiManager::getControl(std::string name, int sess)
{
    if(sess != -1)
    {
        if(m_sessions[sess]->ctrls.find(name) == m_sessions[sess]->ctrls.end())
            throw Exception("Session::GetControl; Control not found (%s)", name.c_str());

        return m_sessions[sess]->ctrls[name];
    }

    else
    {

        if(m_currentSession->second->ctrls.find(name) == m_currentSession->second->ctrls.end())
            throw Exception("Session::GetControl; Control not found (%s); Session #%d", name.c_str(), sess);

        return m_currentSession->second->ctrls[name];
    }

}

Control* GuiManager::releaseControl(std::string name)
{

    if(m_currentSession->second->ctrls.find(name) == m_currentSession->second->ctrls.end())
        throw Exception("Session::ReleaseControl; Control not found (%s); Session #%d", name.c_str(), m_currentSession->first);

    Control * releaseNode = m_currentSession->second->ctrls[name];

    m_currentSession->second->ctrls.erase(name);

    return releaseNode;
}

GridLayout* GuiManager::addGridLayout(Layout::Orientation type, int rows, int cols, float space, Vector2f border)
{

    return 0;
}

Layout* GuiManager::addLayout(Layout::Orientation type, float space, Vector2f border)
{
    Layout* lay = new Layout(m_viewport, space, border, type);

    // Parent
    m_currentSession->second->layouts.push_back(lay);

    if(m_currentSession->second->activeLayout)
        m_currentSession->second->activeLayout->addLayout(lay);

    else
        m_currentSession->second->headLayouts.push_back(lay);

    m_currentSession->second->activeLayout = lay;

    return lay;
}

void GuiManager::addLayoutSpace(Vector2f space)
{

    if(m_currentSession->second->activeLayout)
        m_currentSession->second->activeLayout->addSpace(space);
}

void GuiManager::addLayoutStretchSpace()
{

    if(m_currentSession->second->activeLayout)
        m_currentSession->second->activeLayout->addStretchSpace();
}

Layout* GuiManager::endLayout()
{
    if(m_currentSession->second->activeLayout)
    {
        m_currentSession->second->activeLayout->update();

        m_currentSession->second->activeLayout = m_currentSession->second->activeLayout->getParent();

        if(m_currentSession->second->activeLayout)
            m_currentSession->second->activeLayout->update();
    }

    return m_currentSession->second->activeLayout;
}

void GuiManager::setViewport(Vector2i viewport)
{
    m_viewport = viewport;

    for(Session::Map::iterator i = m_sessions.begin(); i != m_sessions.end(); i++)
    {
        Layout::Array& lay = i->second->layouts;

        if(!lay.empty())
        {
            lay.front()->setScreenSize(m_viewport);
            lay.front()->update();
        }
    }
}

Vector2i GuiManager::getViewport() const
{
    return m_viewport;
}
// Session ---------------------------------------------------------

GuiManager::Session::Session()
{
    guiSkin = NULL;
    activeLayout = NULL;
}

GuiManager::Session::~Session()
{
    for(Control::Map::iterator i = ctrls.begin(); i != ctrls.end(); i++)
        delete i->second;

    for(unsigned int i = 0; i < headLayouts.size(); i++)
        delete headLayouts[i];

    delete guiSkin;
}
