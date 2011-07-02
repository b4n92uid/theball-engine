/*
 * File:   TextBox.h
 * Author: b4n92uid
 *
 * Created on 10 mai 2010, 19:59
 */

#ifndef _TEXTBOX_H
#define	_TEXTBOX_H

#include "Control.h"
#include "Skin.h"

namespace tbe
{
namespace gui
{

/**
 * \brief Controle d'affichage du text pour l'interface graphique
 */
class TextBox : public Control
{
public:
    TextBox();
    TextBox(std::string path, int size);
    ~TextBox();

    void setOffsetLine(unsigned offsetLine);
    unsigned getOffsetLine() const;

    bool onEvent(const EventManager& event);

    void setSkin(const GuiSkin& skin);

    void write(GuiString text);

    void setDefinedSize(bool definedSize);
    bool isDefinedSize() const;

    enum TextAlign
    {
        LEFT,
        CENTER,
    };

    void setTextAlign(TextAlign textAlign);
    TextAlign getTextAlign() const;

    void setArrowTexture(Texture arrowTexture);
    Texture getArrowTexture() const;

protected:
    void prepareDisplay();
    void objectRender();

protected:
    TextAlign m_textAlign;
    bool m_definedSize;
    Texture m_arrowTexture;

private:
    GuiString m_text;
    GuiString m_textDisplay;
    unsigned m_offsetMax;
    unsigned m_offsetLine;
};

}
}

#endif	/* _TEXTBOX_H */
