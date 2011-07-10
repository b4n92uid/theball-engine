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
#include "Rtt.h"

#include "Pencil.h"

namespace tbe
{

class EventManager;

namespace gui
{

struct GuiSkin;

/// \brief Class de bass pour tout les elements de l'interface graphique

class Dimension
{
public:

    Dimension()
    {
        this->x = 0;
        this->y = 0;
        this->w = 0;
        this->h = 0;
    }

    Dimension(float v)
    {
        this->x = v;
        this->y = v;
        this->w = v;
        this->h = v;
    }

    Dimension(float x, float y)
    {
        this->x = x;
        this->y = y;
        this->w = x;
        this->h = y;
    }

    Dimension(float x, float y, float w, float h)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }

    operator Vector2f()
    {
        return Vector2f(x, y);
    }

    float x, y, w, h;
};

enum Alignement
{
    VCENTER = 2, HCENTER = 4, LEFT = 8, RIGHT = 16, TOP = 32, BOTTOM = 64
};

enum Direction
{
    HORIZENTAL = 2, VERTICAL = 4
};

class Control
{
public:
    Control();
    virtual ~Control();

    typedef std::map<std::string, Control*> Map;
    typedef std::vector<Control*> Array;

    /// Fonction appler par GuiManager lors d'un evenement
    virtual bool onEvent(const EventManager&) = 0;

    /// Specifier une apparence du controle
    virtual void setSkin(const GuiSkin&);

    /// Rendue
    void render();

    /// Label du controle
    void setLabel(std::string label);
    std::string getLabel();

    /// Etat du controle (allumer, éteint)
    void setEnable(bool enable);
    bool isEnable() const;

    /// Etat du controle (étirable, fixe)
    void setStretch(bool stretch);
    bool isStretch() const;

    /// Etat du controle (actif, inactife)
    void setActivate(bool activate);
    virtual bool isActivate() const;

    /// Position du controle
    void move(Vector2f relPos);
    virtual void setPos(Vector2f pos);
    virtual Vector2f getPos() const;

    /// Taille du controle
    virtual void setSize(Vector2f size);
    virtual Vector2f getSize() const;

    /// Specifier l'arrier plan du controle
    void setBackground(Texture background);
    Texture getBackground() const;

    void setBackgroundMask(Texture backgroundMask);
    Texture getBackgroundMask() const;

    /// Spécifier la police d'écriture
    void setPencil(Pencil pencil);
    Pencil getPencil() const;

    /// Spécifier l'état de la transparence alpha
    void setEnableAlpha(bool enableAlpha);
    bool isEnableAlpha() const;

    /// Spécifier l'état de l'arrier plan
    void setEnableBackground(bool enableBackground);
    bool isEnableBackground() const;

    void setOpacity(float opacity);
    float getOpacity() const;

    void setTextAlign(unsigned textAlign);
    unsigned getTextAlign() const;

    void setTextPadding(Dimension textPadding);
    Dimension getTextPadding() const;

    void setName(std::string name);
    std::string getName() const;

    void setOutput(Rtt* output);
    Rtt* getOutput() const;

    void setMetaCount(int metaCount);
    int getMetaCount() const;

protected:
    virtual void objectRender() = 0;

    tbe::Vector2f::Array vertexPos(tbe::Vector2f pos, tbe::Vector2f size);
    tbe::Vector2f::Array vertexUv(tbe::Vector2f offset, tbe::Vector2f length);

    void drawBackground();
    void drawSurface(Vector2f pos[], Vector2f st[], unsigned draw);
    void drawSurface(Vector2f pos, Vector2f size, Vector2f tcOffset = 0, Vector2f tcLength = 1);

    std::string m_name;
    std::string m_label;

    bool m_enable;
    bool m_enableAlpha;
    bool m_enableBackground;
    bool m_activate;
    bool m_stretch;

    Vector2f m_pos;
    Vector2f m_size;

    unsigned m_textAlign;
    Dimension m_textPadding;

    float m_opacity;

    int m_metaCount;

    Texture m_background;
    Texture m_backgroundMask;
    Pencil m_pencil;

    Rtt* m_output;
};

}
}

#endif // _CONTROL_H
