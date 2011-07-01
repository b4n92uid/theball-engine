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

    ListBox& push(const std::string& label);
    ListBox& push(const std::string& label, const Any& data);

    void setCurrentIndex(unsigned index);
    unsigned getCurrentIndex();

    void setCurrentString(const std::string& str);
    std::string getCurrentString();

    void setCurrentData(const Any& data);
    Any getCurrentData();

    void setString(unsigned index, const std::string& str);
    std::string getString(unsigned index);

    void setData(unsigned index, const Any& data);
    Any getData(unsigned index);

    unsigned getCount();

    void clear();

    bool onEvent(const EventManager& event);

    void setSkin(const GuiSkin& gui);

    void setBackgroundPadding(Vector2f backgroundPadding);
    Vector2f getBackgroundPadding() const;

    void cancelSelection();
    bool isSelection();

    void setDefinedSize(bool definedSize);
    bool isDefinedSize() const;

    void update();

protected:
    void objectRender();

    class Item : public Control
    {
    public:
        Item(std::string label);
        Item(std::string label, const Any& data);
        ~Item();

        bool onEvent(const EventManager& event);

        void setData(const Any& data);
        Any getData() const;

        void setSelected(bool selected);
        bool isSelected() const;

        void setNotAnItem(bool notAnItem);
        bool isNotAnItem() const;

        typedef std::vector<Item*> Array;

    protected:
        void objectRender();

    protected:
        Any m_data;
        bool m_selected;
        bool m_notAnItem;
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
