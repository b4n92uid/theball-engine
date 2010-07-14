/* 
 * File:   KeyConfig.h
 * Author: b4n92uid
 *
 * Created on 5 février 2010, 11:50
 */

#ifndef _KEYCONFIG_H
#define	_KEYCONFIG_H

#include "Texture.h"

#include "Control.h"
#include "Pencil.h"

namespace tbe
{
namespace gui
{

/**
 * \brief Controle de configuration d'une touche
 * enregistre le nom et le code de la touche entrer
 */

class KeyConfig : public Control
{
public:
    KeyConfig();
    KeyConfig(Pencil font, Texture background);
    ~KeyConfig();

    /// Changer l'apparance du controle
    void SetSkin(const GuiSkin& skin);

    /// Fonction appler lors d'un evenement
    bool OnEvent(const EventManager& event);

    void SetKeyCode(int keyCode, std::string keyName = "");
    int GetKeyCode() const;

    void SetMouseCode(int mouseCode, std::string buttonName = "");
    int GetMouseCode() const;

protected:
    void ObjectRender();

    int m_keyCode;
    int m_mouseCode;

    enum State
    {
        NO_EVENT, OVER, EDIT
    } m_state;
};

}
}

#endif	/* _KEYCONFIG_H */

