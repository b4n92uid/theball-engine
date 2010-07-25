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
#include "VectorBox.h"
#include "ListBox.h"

using namespace tbe;
using namespace tbe::gui;
using namespace std;

// GuiManager ------------------------------------------------------------------

GuiManager::GuiManager()
{
    m_guiSkin = NULL;
    m_currentSession = m_sessions.end();

    SetSession(0);
}

GuiManager::~GuiManager()
{
    ClearAll();

    if(m_guiSkin)
        delete m_guiSkin;
}

void GuiManager::ClearAll()
{
    for(GuiManager::Session::Map::iterator its = m_sessions.begin(); its != m_sessions.end(); its++)
        delete its->second;

    m_sessions.clear();
    m_currentSession = m_sessions.end();
}

void GuiManager::Setup(Vector2i scrsize)
{
    m_screenSize = scrsize;

    for(Session::Map::iterator i = m_sessions.begin(); i != m_sessions.end(); i++)
    {
        Layout::Array& lay = i->second->m_layouts;

        if(!lay.empty())
        {
            lay.front()->SetScreenSize(m_screenSize);
            lay.front()->Update();
        }
    }
}

void GuiManager::Render()
{
    // Passage en Ortho 2D
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    glLoadIdentity();
    gluOrtho2D(0, m_screenSize.x, 0, m_screenSize.y);

    // --
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_TEXTURE_2D);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    for_each(m_currentSession->second->m_headLayouts.begin(),
             m_currentSession->second->m_headLayouts.end(),
             mem_fun(&Layout::Update));

    for(Control::Map::iterator itt = m_currentSession->second->m_ctrls.begin();
        itt != m_currentSession->second->m_ctrls.end(); itt++)
        itt->second->Render();

    glPopAttrib();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void GuiManager::PageUp()
{
    m_currentSession--;

    if(m_currentSession == --m_sessions.begin())
        m_currentSession = --m_sessions.end();
}

void GuiManager::PageDown()
{
    m_currentSession++;

    if(m_currentSession == m_sessions.end())
        m_currentSession = m_sessions.begin();
}

void GuiManager::SetSession(unsigned sessID)
{
    if(m_currentSession->first != sessID && m_currentSession != m_sessions.end())
        for(Control::Map::iterator itt = m_currentSession->second->m_ctrls.begin();
            itt != m_currentSession->second->m_ctrls.end(); itt++)
            itt->second->SetActivate(false);

    if(m_sessions.find(sessID) == m_sessions.end())
        m_sessions[sessID] = new Session;

    m_currentSession = m_sessions.find(sessID);
}

unsigned GuiManager::GetSession()
{
    return m_currentSession->first;
}

void GuiManager::DestroySession(unsigned sessID)
{
    if(m_currentSession->first == sessID)
        m_currentSession++;

    delete m_sessions[sessID];
    m_sessions.erase(sessID);

    if(m_currentSession == m_sessions.end())
        if(m_sessions.size() > 0)
            m_currentSession = m_sessions.begin();
}

void GuiManager::TrasmitEvent(EventManager& e)
{
    if(m_currentSession != m_sessions.end())
        for(Control::Map::iterator it = m_currentSession->second->m_ctrls.begin();
            it != m_currentSession->second->m_ctrls.end(); it++)
            if(it->second->OnEvent(e))
            {
                e.notify = EventManager::EVENT_NO_EVENT;
                break;
            }
}

void GuiManager::SetSkin(GuiSkin* skin)
{
    if(m_guiSkin)
        delete m_guiSkin;

    m_guiSkin = skin;

    if(m_guiSkin)
        for(Session::Map::iterator i = m_sessions.begin(); i != m_sessions.end(); i++)
            for(Control::Map::iterator j = i->second->m_ctrls.begin(); j != i->second->m_ctrls.end(); j++)
                j->second->SetSkin(*m_guiSkin);

}

TextBox* GuiManager::AddTextBox(std::string name)
{
    TextBox* p = new TextBox;

    AddControl(name, p);

    return p;
}

KeyConfig* GuiManager::AddKeyConfig(std::string name)
{
    KeyConfig* p = new KeyConfig;

    AddControl(name, p);

    return p;
}

Gauge* GuiManager::AddGauge(std::string name, std::string label)
{
    Gauge* p = new Gauge;
    p->SetLabel(label);

    AddControl(name, p);

    return p;
}

EditBox* GuiManager::AddEditBox(std::string name, std::string label)
{
    EditBox* p = new EditBox;
    p->SetLabel(label);

    AddControl(name, p);

    return p;
}

SwitchString* GuiManager::AddSwitchString(std::string name)
{
    SwitchString* p = new SwitchString;

    AddControl(name, p);

    return p;
}

Image* GuiManager::AddImage(std::string name, Texture path)
{

    Image* p = new Image(path);

    AddControl(name, p);

    return p;
}

StateShow* GuiManager::AddStateShow(std::string name, Texture path, int stateCount)
{

    StateShow* p = new StateShow;
    p->SetBackground(path);
    p->SetStateCount(stateCount);

    AddControl(name, p);

    return p;
}

ListBox* GuiManager::AddListBox(std::string name)
{
    ListBox* p = new ListBox;

    AddControl(name, p);

    return p;
}

VectorBox* GuiManager::AddVectorBox(std::string name, Vector3f value)
{
    VectorBox* p = new VectorBox;
    p->SetValue(value);

    AddControl(name, p);

    return p;
}

Button* GuiManager::AddButton(std::string name, std::string label)
{
    Button* p = new Button;
    p->SetLabel(label);

    AddControl(name, p);

    return p;
}

void GuiManager::AddControl(std::string name, Control* ctrl)
{
    if(name.empty())
        name = tools::NameGen(m_currentSession->second->m_ctrls);

    if(m_currentSession->second->m_ctrls.find(name) != m_currentSession->second->m_ctrls.end())
        throw Exception("GuiManager::AddControl; Control already exist (%s)", name.c_str());

    m_currentSession->second->m_ctrls[name] = ctrl;

    if(m_guiSkin)
        ctrl->SetSkin(*m_guiSkin);

    ctrl->SetName(name);

    if(m_currentSession->second->m_activeLayout)
        m_currentSession->second->m_activeLayout->AddControl(ctrl);
}

void GuiManager::DeleteControl(std::string name)
{

    if(m_currentSession->second->m_ctrls.find(name) == m_currentSession->second->m_ctrls.end())
        throw Exception("GuiManager::DeleteControl; Control not found (%s); Session #%d", name.c_str(), m_currentSession->first);

    delete m_currentSession->second->m_ctrls[name];
    m_currentSession->second->m_ctrls.erase(name);
}

Control* GuiManager::GetControl(std::string name, int sess)
{
    if(sess != -1)
    {
        if(m_sessions[sess]->m_ctrls.find(name) == m_sessions[sess]->m_ctrls.end())
            throw Exception("Session::GetControl; Control not found (%s)", name.c_str());

        return m_sessions[sess]->m_ctrls[name];
    }

    else
    {

        if(m_currentSession->second->m_ctrls.find(name) == m_currentSession->second->m_ctrls.end())
            throw Exception("Session::GetControl; Control not found (%s); Session #%d", name.c_str(), sess);

        return m_currentSession->second->m_ctrls[name];
    }

}

Control* GuiManager::ReleaseControl(std::string name)
{

    if(m_currentSession->second->m_ctrls.find(name) == m_currentSession->second->m_ctrls.end())
        throw Exception("Session::ReleaseControl; Control not found (%s); Session #%d", name.c_str(), m_currentSession->first);

    Control * releaseNode = m_currentSession->second->m_ctrls[name];

    m_currentSession->second->m_ctrls.erase(name);

    return releaseNode;
}

GridLayout* GuiManager::AddGridLayout(Layout::Orientation type, int rows, int cols, float space, Vector2f border)
{

    return 0;
}

Layout* GuiManager::AddLayout(Layout::Orientation type, float space, Vector2f border)
{
    Layout* lay = new Layout(m_screenSize, border, space, type);

    // Parent
    m_currentSession->second->m_layouts.push_back(lay);

    if(m_currentSession->second->m_activeLayout)
        m_currentSession->second->m_activeLayout->AddLayout(lay);

    else
        m_currentSession->second->m_headLayouts.push_back(lay);

    m_currentSession->second->m_activeLayout = lay;

    return lay;
}

void GuiManager::AddLayoutSpace(Vector2f space)
{

    if(m_currentSession->second->m_activeLayout)
        m_currentSession->second->m_activeLayout->AddSpace(space);
}

void GuiManager::AddLayoutStretchSpace()
{

    if(m_currentSession->second->m_activeLayout)
        m_currentSession->second->m_activeLayout->AddStretchSpace();
}

Layout* GuiManager::EndLayout()
{
    if(m_currentSession->second->m_activeLayout)
    {
        m_currentSession->second->m_activeLayout->Update();

        m_currentSession->second->m_activeLayout = m_currentSession->second->m_activeLayout->GetParent();

        if(m_currentSession->second->m_activeLayout)
            m_currentSession->second->m_activeLayout->Update();
    }

    return m_currentSession->second->m_activeLayout;
}

Vector2f GuiManager::GetScreenSize() const
{
    return m_screenSize;
}

// Session ---------------------------------------------------------

GuiManager::Session::Session()
{

    m_activeLayout = NULL;
}

GuiManager::Session::~Session()
{
    for(Control::Map::iterator i = m_ctrls.begin(); i != m_ctrls.end(); i++)
        delete i->second;

    for(unsigned int i = 0; i < m_headLayouts.size(); i++)
        delete m_headLayouts[i];
}
