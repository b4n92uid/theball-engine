/* 
 * File:   ListBox.h
 * Author: b4n92uid
 *
 * Created on 15 juin 2010, 21:41
 */

#ifndef _LISTBOX_H
#define	_LISTBOX_H

#include "Control.h"
#include "Skin.h"
#include "Layout.h"

namespace tbe
{
namespace gui
{

/// \brief List d'élément

class ListBox : public Control
{
public:
    ListBox();
    ~ListBox();

    ListBox& Push(const std::string& label);
    ListBox& Push(const std::string& label, const Any& data);

    void SetCurrentIndex(unsigned index);
    unsigned GetCurrentIndex();

    void SetCurrentString(const std::string& str);
    std::string GetCurrentString();

    void SetCurrentData(const Any& data);
    Any GetCurrentData();

    void SetString(unsigned index, const std::string& str);
    std::string GetString(unsigned index);

    void SetData(unsigned index, const Any& data);
    Any GetData(unsigned index);

    unsigned GetCount();

    void Clear();

    bool OnEvent(const EventManager& event);

    void SetSkin(const GuiSkin& gui);

    void SetBackgroundPadding(Vector2f backgroundPadding);
    Vector2f GetBackgroundPadding() const;

    void CancelSelection();
    bool IsSelection();

    void SetDefinedSize(bool definedSize);
    bool IsDefinedSize() const;

    void Update();

protected:
    void ObjectRender();

    class Item : public Control
    {
    public:
        Item(std::string label);
        Item(std::string label, const Any& data);
        ~Item();

        bool OnEvent(const EventManager& event);

        void SetData(const Any& data);
        Any GetData() const;

        void SetSelected(bool selected);
        bool IsSelected() const;

        typedef std::vector<Item*> Array;

    protected:
        void ObjectRender();

    protected:
        Any m_data;
        bool m_selected;
    };

    Item::Array m_displayItems;
    Item::Array m_totalItems;
    Item* m_currentItem;
    Layout m_lay;

    unsigned m_offset;

    bool m_definedSize;

    unsigned m_offsetMax;

    Vector2f m_backgroundPadding;
};

}
}

#endif	/* _LISTBOX_H */
