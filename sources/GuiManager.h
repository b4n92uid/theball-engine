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
    void Setup(Vector2i scrsize);

    /// Efface tout les controles
    void ClearAll();

    /// Rendue
    void Render();

    /// Transmition des evenement aux controle
    void TrasmitEvent(EventManager& e);

    /// Change l'apparence des controle
    void SetSkin(GuiSkin* skin);

    /// Génére/Sélection une session
    void SetSession(unsigned sessID);

    /// Séléctione la session suivante
    void PageUp();

    /// Séléctione la session précédente
    void PageDown();

    /// Retourne l'id de la session courant
    unsigned GetSession();

    /// Détruite la session sees
    void DestroySession(unsigned name);

    /// Ajout le control ListBox
    ListBox* AddListBox(std::string name);

    /// Ajout le control VectorBox
    VectorBox* AddVectorBox(std::string name, Vector3f value);

    /// Ajout le control TextFont
    TextBox* AddTextBox(std::string name);

    /// Ajout le control Gauge
    Gauge* AddGauge(std::string name, std::string label);

    /// Ajout le control KeyConfig
    KeyConfig* AddKeyConfig(std::string name);

    /// Ajout le control EditBox
    EditBox* AddEditBox(std::string name, std::string label);

    /// Ajout le control Switch
    SwitchString* AddSwitchString(std::string name);

    /// Ajout le control SwitchNumeric

    template<typename T> SwitchNumeric<T>* AddSwitchNumeric(std::string name)
    {
        SwitchNumeric<T>* sn = new SwitchNumeric<T>;
        AddControl(name, sn);
        return sn;
    }

    /// Ajout le control Button
    Button* AddButton(std::string name, std::string label);

    /// Ajout le control Image
    Image* AddImage(std::string name, Texture path = Texture());

    // Ajout le controle StateShow
    StateShow* AddStateShow(std::string name, Texture path, int stateCount);

    /// Ajout un control
    void AddControl(std::string name, Control* ctrl);

    /// Accès aux control de la session courante
    Control* GetControl(std::string name, int sess = -1);

    /// Détrute un controle de la session courante
    void DeleteControl(std::string name);

    // Détache un controle de la session courante
    Control* ReleaseControl(std::string name);

    /// Créer un layout
    GridLayout* AddGridLayout(Layout::Orientation type, int rows, int cols, float space = 0, Vector2f border = 0);

    /// Créer un layout
    Layout* AddLayout(Layout::Orientation type, float space = 0, Vector2f border = 0);

    /// Termine le layout créer
    Layout* EndLayout();

    /// Ajoute un espace etirable aux layout déja créer
    void AddLayoutStretchSpace();

    /// Ajoute un espace fixe aux layout déja créer
    void AddLayoutSpace(Vector2f space);

    Vector2f GetScreenSize() const;

    template<typename T> T* ReleaseControl(std::string name)
    {
        return(T*)ReleaseControl(name);
    }

    template<typename T> T* GetControl(std::string name, int sess = -1)
    {
        return(T*)GetControl(name, sess);
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
