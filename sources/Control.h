#ifndef _CONTROL_H
#define _CONTROL_H

#include "GLee.h"

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <cstdarg>
#include <exception>

#include "Any.h"
#include "Texture.h"
#include "Mathematics.h"
#include "Eventrecv.h"
#include "Exception.h"

#include "Pencil.h"

namespace tbe
{

class EventManager;

namespace gui
{

struct GuiSkin;

/// \brief Class de bass pour tout les elements de l'interface graphique

class Control
{
public:
    Control();
    virtual ~Control();

    typedef std::map<std::string, Control*> Map;
    typedef std::vector<Control*> Array;

    /// Fonction appler par GuiManager lors d'un evenement
    virtual bool OnEvent(const EventManager&) = 0;

    /// Specifier une apparence du controle
    virtual void SetSkin(const GuiSkin&);

    /// Rendue
    void Render();

    /// Label du controle
    void SetLabel(std::string label);
    std::string GetLabel();

    /// Etat du controle (allumer, éteint)
    void SetEnable(bool enable);
    bool IsEnable() const;

    /// Etat du controle (étirable, fixe)
    void SetStretch(bool stretch);
    bool IsStretch() const;

    /// Etat du controle (actif, inactife)
    void SetActivate(bool activate);
    virtual bool IsActivate() const;

    /// Position du controle
    void Move(Vector2f relPos);
    virtual void SetPos(Vector2f pos);
    virtual Vector2f GetPos() const;

    /// Taille du controle
    virtual void SetSize(Vector2f size);
    virtual Vector2f GetSize() const;

    /// Specifier l'arrier plan du controle
    void SetBackground(Texture background);
    Texture GetBackground() const;

    /// Spécifier la police d'écriture
    void SetPencil(Pencil pencil);
    Pencil GetPencil() const;

    /// Spécifier l'état de la transparence alpha
    void SetEnableAlpha(bool enableAlpha);
    bool IsEnableAlpha() const;

    /// Spécifier l'état de l'arrier plan
    void SetEnableBackground(bool enableBackground);
    bool IsEnableBackground() const;

    void SetOpacity(float opacity);
    float GetOpacity() const;

    void SetName(std::string name);
    std::string GetName() const;

protected:
    virtual void ObjectRender() = 0;

    void DrawSurface(Vector2f pos[], Vector2f st[], unsigned draw);
    void DrawSurface(Vector2f pos, Vector2f size, Vector2f tcOffset, Vector2f tcLength);

    std::string m_name;
    std::string m_label;

    bool m_enable;
    bool m_enableAlpha;
    bool m_enableBackground;
    bool m_activate;
    bool m_stretch;

    Vector2f m_pos;
    Vector2f m_size;

    float m_opacity;

    Texture m_background;
    Pencil m_pencil;
};

}
}

#endif // _CONTROL_H
