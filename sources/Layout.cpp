#include "Layout.h"

using namespace tbe;
using namespace tbe::gui;

class Spacer : public Control
{
public:

    Spacer()
    {
        m_size = 1;
    }

    bool OnEvent(const EventManager&)
    {
        return false;
    }

    void ObjectRender()
    {
    }
};

Layout::Layout(Vector2f scrsize, Vector2f border, float space, Orientation type)
{
    m_screenSize = scrsize;
    m_border = border;
    m_space = space;
    m_orientation = type;
    m_align = BEFORE;

    m_parent = NULL;
}

Layout::~Layout()
{
    Clear();
}

void Layout::Clear()
{
    for(unsigned i = 0; i < m_childControlsTmp.size(); i++)
        delete m_childControlsTmp[i];

    m_childControlsTmp.clear();

    for(unsigned i = 0; i < m_childControls.size(); i++)
        delete m_childControls[i];

    m_childControls.clear();

    for(unsigned i = 0; i < m_childLayout.size(); i++)
        delete m_childLayout[i];

    m_childLayout.clear();
}

void Layout::AddStretchSpace()
{
    Spacer* object = new Spacer;
    object->SetSize(1);
    object->SetStretch(true);

    m_childControls.push_back(new Item(object));
    m_childControlsTmp.push_back(new Item(object));
}

void Layout::AddSpace(Vector2f size)
{
    Spacer* object = new Spacer;
    object->SetSize(size);

    m_childControls.push_back(new Item(object));
    m_childControlsTmp.push_back(new Item(object));
}

void Layout::AddLayout(Layout* object)
{
    object->m_screenSize -= m_border * 2.0f;
    object->m_parent = this;

    m_childLayout.push_back(object);
    m_childControls.push_back(new Item(object));
}

void Layout::ReleaseControl(Control* object)
{
    using namespace std;

    Item::Array::iterator it = find_if(m_childControls.begin(),
                                       m_childControls.end(),
                                       bind2nd(ptr_fun(&Item::FindByControl), object));

    if(it == m_childControls.end())
        throw Exception("Layout::ReleaseControl; Control not found (%p)", object);

    m_childControls.erase(it);
}

void Layout::InsertFront(Control* object)
{
    m_childControls.insert(m_childControls.begin(), new Item(object));
}

void Layout::AddControl(Control* object)
{
    m_childControls.push_back(new Item(object));
}

void Layout::Update()
{

    for(unsigned i = 0; i < m_childLayout.size(); i++)
        m_childLayout[i]->Update();

    if(m_childControls.empty())
        return;

    m_size = 0;

    if(m_orientation == Vertical)
    {
        float noneStretchControlSize = 0; // Taille de tout les objet NON m_stretch
        float stretchControlSize = 0; // Taille de tout les objet m_stretch
        int stretchControlCount = 0; // Nombre d'objet m_stretch

        // On calcule la Taille de tout les objet NON m_stretch

        for(unsigned i = 0; i < m_childControls.size(); i++)
        {
            if(m_childControls[i]->IsStretch())
                stretchControlCount++;

            else
                noneStretchControlSize += m_childControls[i]->GetSize().y + m_space;
        }

        noneStretchControlSize -= m_space;

        // Taille de tout les objet m_stretch

        if(!stretchControlCount)
            stretchControlCount = 1;

        stretchControlSize = (m_screenSize.y - m_border.y * 2 - noneStretchControlSize) / stretchControlCount;

        // Mise a jour des position
        for(unsigned i = 0; i < m_childControls.size(); i++)
        {
            if(m_childControls[i]->IsStretch())
            {
                Vector2f newSize = m_childControls[i]->GetSize();
                newSize.y = stretchControlSize;

                if(newSize.x <= 0) newSize.x = 1;
                if(newSize.y <= 0) newSize.y = 1;

                m_childControls[i]->SetSize(newSize);
            }

            if(!i)
            {
                Vector2f pos(m_pos.x + m_border.x, m_pos.y + m_border.y);

                m_childControls[i]->SetPos(pos);
            }

            else
            {
                Vector2f pos = m_childControls[i - 1]->GetPos();
                Vector2f size = m_childControls[i - 1]->GetSize();

                if(!m_childControls[i - 1]->IsStretch())
                    size.y += m_space;

                m_childControls[i]->SetPos(Vector2f(pos.x, pos.y + size.y));

                m_size.y += size.y;

                if(m_size.x < size.x)
                    m_size.x = size.x;
            }

        }

        Vector2f size = m_childControls.back()->GetSize();

        if(m_size.x < size.x)
            m_size.x = size.x;

        // Alignement
        if(m_align != BEFORE)
            for(unsigned i = 0; i < m_childControls.size(); i++)
            {
                Vector2f pos = m_childControls[i]->GetPos();
                Vector2f size = m_childControls[i]->GetSize();

                if(m_align == AFTER)
                    pos.x = pos.x + m_size.x - size.x;
                else if(m_align == MIDDLE)
                    pos.x = pos.x + (m_size.x / 2 - size.x / 2);

                m_childControls[i]->SetPos(pos);
            }
    }

    else if(m_orientation == Horizental)
    {
        float noneStretchControlSize = 0; // Taille de tout les objet NON m_stretch
        float stretchControlSize = 0; // Taille de tout les objet m_stretch
        int stretchControlCount = 0; // Nombre d'objet m_stretch

        // On calcule la Taille de tout les objet NON m_stretch

        for(unsigned i = 0; i < m_childControls.size(); i++)
        {
            if(m_childControls[i]->IsStretch())
                stretchControlCount++;

            else
                noneStretchControlSize += m_childControls[i]->GetSize().x + m_space;
        }

        noneStretchControlSize -= m_space;

        // Taille de tout les objet m_stretch

        if(!stretchControlCount)
            stretchControlCount = 1;

        stretchControlSize = (m_screenSize.x - m_border.x * 2 - noneStretchControlSize) / stretchControlCount;

        // Mise a jour des position
        for(unsigned i = 0; i < m_childControls.size(); i++)
        {
            if(m_childControls[i]->IsStretch())
            {
                Vector2f newSize = m_childControls[i]->GetSize();
                newSize.x = stretchControlSize;

                if(newSize.x <= 0) newSize.x = 1;
                if(newSize.y <= 0) newSize.y = 1;

                m_childControls[i]->SetSize(newSize);
            }

            if(!i)
            {
                Vector2f newSize(m_pos.x + m_border.x, m_pos.y + m_border.y);

                m_childControls[i]->SetPos(newSize);
            }

            else
            {
                Vector2f pos = m_childControls[i - 1]->GetPos();
                Vector2f size = m_childControls[i - 1]->GetSize();

                if(!m_childControls[i - 1]->IsStretch())
                    size.x += m_space;

                m_childControls[i]->SetPos(Vector2f(pos.x + size.x, pos.y));

                m_size.x += size.x;

                if(m_size.y < size.y)
                    m_size.y = size.y;
            }
        }

        Vector2f size = m_childControls.back()->GetSize();

        if(m_size.y < size.y)
            m_size.y = size.y;

        // Alignement
        if(m_align != BEFORE)
            for(unsigned i = 0; i < m_childControls.size(); i++)
            {
                Vector2f pos = m_childControls[i]->GetPos();
                Vector2f size = m_childControls[i]->GetSize();

                if(m_align == AFTER)
                    pos.y = pos.y + m_size.y - size.y;
                else if(m_align == MIDDLE)
                    pos.y = pos.y + (m_size.y / 2 - size.y / 2);

                m_childControls[i]->SetPos(pos);
            }
    }
}

void Layout::SetAlign(Align align)
{
    this->m_align = align;
}

Layout::Align Layout::GetAlign() const
{
    return m_align;
}

void Layout::SetParent(Layout* parent)
{
    this->m_parent = parent;
}

Layout* Layout::GetParent()
{
    return m_parent;
}

void Layout::SetSpace(float size)
{
    m_space = size;
}

float Layout::GetSpace() const
{
    return m_space;
}

void Layout::SetBorder(Vector2f size)
{
    m_border = size;
}

Vector2f Layout::GetBorder() const
{
    return m_border;
}

void Layout::SetScreenSize(Vector2f screenSize)
{
    this->m_screenSize = screenSize;
}

Vector2f Layout::GetScreenSize() const
{
    return m_screenSize;
}

void Layout::SetSize(Vector2f size)
{
    this->m_size = size;
}

Vector2f Layout::GetSize() const
{
    return m_size;
}

void Layout::SetPos(Vector2f pos)
{
    this->m_pos = pos;

    for(unsigned i = 0; i < m_childLayout.size(); i++)
        m_childLayout[i]->SetPos(m_pos + m_childLayout[i]->GetPos());
}

Vector2f Layout::GetPos() const
{
    return m_pos;
}

void Layout::SetOrientation(Layout::Orientation type)
{
    m_orientation = type;
}

Layout::Orientation Layout::GetOrientation() const
{
    return m_orientation;
}

// Layout::Item ----------------------------------------------------------------

Layout::Item::Item(Layout* layout)
{
    m_ctrl = 0;
    m_lay = layout;
}

Layout::Item::Item(Control* ctrl)
{
    m_ctrl = ctrl;
    m_lay = 0;
}

Layout::Item::~Item()
{

}

Vector2f Layout::Item::GetPos()
{
    return m_ctrl ? m_ctrl->GetPos() : m_lay->GetPos();
}

void Layout::Item::SetPos(Vector2f pos)
{
    m_ctrl ? m_ctrl->SetPos(pos) : m_lay->SetPos(pos);
}

Vector2f Layout::Item::GetSize()
{
    return m_ctrl ? m_ctrl->GetSize() : m_lay->GetSize();
}

void Layout::Item::SetSize(Vector2f size)
{
    m_ctrl ? m_ctrl->SetSize(size) : m_lay->SetSize(size);
}

bool Layout::Item::IsStretch()
{
    return m_ctrl ? m_ctrl->IsStretch() : false;
}

bool Layout::Item::IsEnable()
{
    return m_ctrl ? m_ctrl->IsEnable() : true;
}
