#ifndef tbe_GUI_H
#define tbe_GUI_H

#include <iostream>
#include <string>
#include <map>

#include "GLee.h"

#include "Mathematics.h"
#include "Texture.h"

#include "Control.h"
#include "Layout.h"
#include "Switch.h"

namespace tbe
{

class Device;
class EventManager;

namespace gui
{

class GridLayout;
class EditBox;
class StateShow;
class Switch;
class SwitchString;
class Button;
class TextBox;
class Image;
class GuiSkin;
class Gauge;
class KeyConfig;

/// \brief Gestionnaire de l'interface graphique

class GuiManager
{
public:

    /// Ctor
    GuiManager();

    /// Dtor
    ~GuiManager();

    /// Initialisation 2D
    void setup(Vector2i scrsize);

    /// Efface tout les controles
    void clearAll();

    /// Rendue
    void render();

    /// Transmition des evenement aux controle
    void trasmitEvent(EventManager& e);

    /// Change l'apparence des controle
    void setSkin(GuiSkin* skin);

    /// G�n�re/S�lection une session
    void setSession(unsigned sessID, GuiSkin* skin = NULL);

    /// S�l�ctione la session suivante
    void pageUp();

    /// S�l�ctione la session pr�c�dente
    void pageDown();

    /// Retourne l'id de la session courant
    unsigned getSession();

    /// D�truite la session sees
    void destroySession(unsigned name);

    /// Ajout le control TextFont
    TextBox* addTextBox(std::string name);

    /// Ajout le control Gauge
    Gauge* addGauge(std::string name, std::string label);

    /// Ajout le control KeyConfig
    KeyConfig* addKeyConfig(std::string name);

    /// Ajout le control EditBox
    EditBox* addEditBox(std::string name, std::string label);

    /// Ajout le control Switch
    SwitchString* addSwitchString(std::string name);

    /// Ajout le control SwitchNumeric

    template<typename T> SwitchNumeric<T>* addSwitchNumeric(std::string name)
    {
        SwitchNumeric<T>* sn = new SwitchNumeric<T>;
        addControl(name, sn);
        return sn;
    }

    /// Ajout le control Button
    Button* addButton(std::string name, std::string label);

    /// Ajout le control Image
    Image* addImage(std::string name, Texture path = Texture());

    // Ajout le controle StateShow
    StateShow* addStateShow(std::string name, Texture path, int metaCount);

    /// Ajout un control
    void addControl(std::string name, Control* ctrl);

    /// Acc�s aux control de la session courante
    Control* getControl(std::string name, int sess = -1);

    /// D�trute un controle de la session courante
    void deleteControl(std::string name);

    /// D�trute un controle de tout les session
    void deleteControls(Control* ctrl);

    // D�tache un controle de la session courante
    Control* releaseControl(std::string name);

    /// Cr�er un layout
    GridLayout* addGridLayout(Layout::Orientation type, int rows, int cols, float space = 0, Vector2f border = 0);

    /// Cr�er un layout
    Layout* addLayout(Layout::Orientation type, float space = 0, Vector2f border = 0);

    /// Termine le layout cr�er
    Layout* endLayout();

    /// Ajoute un espace etirable aux layout d�ja cr�er
    void addLayoutStretchSpace();

    /// Ajoute un espace fixe aux layout d�ja cr�er
    void addLayoutSpace(Vector2f space);

    void setViewport(Vector2i viewport);
    Vector2i getViewport() const;

    void updateLayout();

    template<typename T> T* releaseControl(std::string name)
    {
        return(T*)releaseControl(name);
    }

    template<typename T> T* getControl(std::string name, int sess = -1)
    {
        return(T*)getControl(name, sess);
    }

protected:

    struct Session
    {
        Session();
        ~Session();

        Layout::Array headLayouts;
        Layout::Array layouts;
        Control::Map ctrls;
        Layout* activeLayout;
        GuiSkin* guiSkin;

        typedef std::map<unsigned, Session*> Map;
    };


    Session::Map m_sessions;
    Session::Map::iterator m_currentSession;

    GuiSkin* m_guiSkin;

    Vector2i m_viewport;

    float m_sizeFactor;
};

}
}

#endif
