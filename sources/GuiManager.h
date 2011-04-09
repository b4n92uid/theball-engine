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
class VectorBox;
class ListBox;

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

    /// Génére/Sélection une session
    void setSession(unsigned sessID);

    /// Séléctione la session suivante
    void pageUp();

    /// Séléctione la session précédente
    void pageDown();

    /// Retourne l'id de la session courant
    unsigned getSession();

    /// Détruite la session sees
    void destroySession(unsigned name);

    /// Ajout le control ListBox
    ListBox* addListBox(std::string name);

    /// Ajout le control VectorBox
    VectorBox* addVectorBox(std::string name, Vector3f value);

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
    StateShow* addStateShow(std::string name, Texture path, int stateCount);

    /// Ajout un control
    void addControl(std::string name, Control* ctrl);

    /// Accès aux control de la session courante
    Control* getControl(std::string name, int sess = -1);

    /// Détrute un controle de la session courante
    void deleteControl(std::string name);

    // Détache un controle de la session courante
    Control* releaseControl(std::string name);

    /// Créer un layout
    GridLayout* addGridLayout(Layout::Orientation type, int rows, int cols, float space = 0, Vector2f border = 0);

    /// Créer un layout
    Layout* addLayout(Layout::Orientation type, float space = 0, Vector2f border = 0);

    /// Termine le layout créer
    Layout* endLayout();

    /// Ajoute un espace etirable aux layout déja créer
    void addLayoutStretchSpace();

    /// Ajoute un espace fixe aux layout déja créer
    void addLayoutSpace(Vector2f space);

    Vector2f getScreenSize() const;

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

        Layout::Array m_headLayouts;
        Layout::Array m_layouts;
        Control::Map m_ctrls;
        Layout* m_activeLayout;

        typedef std::map<unsigned, Session*> Map;
    };


    Session::Map m_sessions;
    Session::Map::iterator m_currentSession;

    GuiSkin* m_guiSkin;

    Vector2f m_screenSize;

    float m_sizeFactor;
};

}
}

#endif
