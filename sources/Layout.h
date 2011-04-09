#ifndef LAYOUT_H
#define LAYOUT_H

#include "Control.h"

namespace tbe
{
namespace gui
{

/// \brief Gestion de l'alingement des controle de l'interface graphique

class Layout
{
public:

    typedef std::vector<Layout*> Array;

    enum Orientation
    {
        Vertical,
        Horizental
    };

    /// Ctor
    Layout(Vector2f scrsize = Vector2f(800, 600), Vector2f border = 0, float space = 0, Orientation type = Vertical);

    /// Dtor
    virtual ~Layout();

    /// Détruite tout les controls et layout contenue
    void clear();

    /// Déttache un cotrole du layout
    void releaseControl(Control* object);

    /// Insere un control en tete de list
    void insertFront(Control* object);

    /// Ajoute un controle
    void addControl(Control* object);

    /// Ajoute un layout
    void addLayout(Layout* object);

    /// Ajoute un espcae fixe
    void addSpace(Vector2f size);

    /// Ajoute un espcae étirable
    void addStretchSpace();

    /// Specifier une orientation
    void setOrientation(Orientation type);
    Orientation getOrientation() const;

    /// Accès au parent
    void setParent(Layout* parent);
    Layout* getParent();

    /// Specifier la taille des bordure
    void setBorder(Vector2f size);
    Vector2f getBorder() const;

    /// Specifier l'espace entre les controle
    void setSpace(float size);
    float getSpace() const;

    /// Taille de l'ecran
    void setScreenSize(Vector2f screenSize);
    Vector2f getScreenSize() const;

    /// Taille du Layout
    void setSize(Vector2f size);
    Vector2f getSize() const;

    /// Positione du Layout
    void setPos(Vector2f pos);
    Vector2f getPos() const;

    /// Mise a jour du Layout
    void update();

    enum Align
    {
        AFTER,
        MIDDLE,
        BEFORE
    };

    void setAlign(Align align);
    Align getAlign() const;

protected:

    Align m_align;
    Orientation m_orientation;

    class Item
    {
    public:
        Item(Layout* layout);
        Item(Control* ctrl);
        virtual ~Item();

        Vector2f GetPos();
        void SetPos(Vector2f pos);

        Vector2f GetSize();
        void SetSize(Vector2f size);

        bool IsStretch();

        bool IsEnable();

        typedef std::vector<Item*> Array;

        static inline bool FindByControl(const Item* item,const Control* ctrl)
        {
            return(item->m_ctrl == ctrl);
        }

    private:
        Control* m_ctrl;
        Layout* m_lay;
    };

    Item::Array m_childControlsTmp;
    Item::Array m_childControls;

    Array m_childLayout;

    Layout* m_parent;

    Vector2f m_pos;
    Vector2f m_size;

    Vector2f m_screenSize;
    Vector2f m_border;
    float m_space;
};

}
}

#endif
