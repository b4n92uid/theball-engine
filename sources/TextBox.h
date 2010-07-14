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

    void SetBackgroundPadding(Vector2f backgroundPadding);
    Vector2f GetBackgroundPadding() const;

    void SetOffsetLine(unsigned offsetLine);
    unsigned GetOffsetLine() const;

    bool OnEvent(const EventManager& event);

    void SetSkin(const GuiSkin& skin);

    void Write(Text text);

    void SetDefinedSize(bool definedSize);
    bool IsDefinedSize() const;

    enum TextAlign
    {
        LEFT,
        CENTER,
    };

    void SetTextAlign(TextAlign textAlign);
    TextAlign GetTextAlign() const;
    
    void SetTextureRepeat(Vector2f textureRepeat);
    Vector2f GetTextureRepeat() const;

protected:
    void PrepareDisplay();
    void ObjectRender();

protected:
    unsigned m_offsetLine;
    Vector2f m_backgroundPadding;
    TextAlign m_textAlign;
    bool m_definedSize;
    Vector2f m_textureRepeat;

private:
    Text m_text;
    Text m_textDisplay;
    unsigned m_offsetMax;
};

}
}

#endif	/* _TEXTBOX_H */
