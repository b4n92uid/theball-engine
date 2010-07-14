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
    void Clear();

    /// Déttache un cotrole du layout
    void ReleaseControl(Control* object);

    /// Insere un control en tete de list
    void InsertFront(Control* object);

    /// Ajoute un controle
    void AddControl(Control* object);

    /// Ajoute un layout
    void AddLayout(Layout* object);

    /// Ajoute un espcae fixe
    void AddSpace(Vector2f size);

    /// Ajoute un espcae étirable
    void AddStretchSpace();

    /// Specifier une orientation
    void SetOrientation(Orientation type);
    Orientation GetOrientation() const;

    /// Accès au parent
    void SetParent(Layout* parent);
    Layout* GetParent();

    /// Specifier la taille des bordure
    void SetBorder(Vector2f size);
    Vector2f GetBorder() const;

    /// Specifier l'espace entre les controle
    void SetSpace(float size);
    float GetSpace() const;

    /// Taille de l'ecran
    void SetScreenSize(Vector2f screenSize);
    Vector2f GetScreenSize() const;

    /// Taille du Layout
    void SetSize(Vector2f size);
    Vector2f GetSize() const;

    /// Positione du Layout
    void SetPos(Vector2f pos);
    Vector2f GetPos() const;

    /// Mise a jour du Layout
    void Update();

    enum Align
    {
        AFTER,
        MIDDLE,
        BEFORE
    };

    void SetAlign(Align align);
    Align GetAlign() const;

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
