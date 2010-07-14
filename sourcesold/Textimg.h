#ifndef TEXTIMG_H
#define TEXTIMG_H

#include "gl_guiobject.h"
#include "gl_texture.h"

class TextImg : public GuiObject
{
    public:

	TextImg::TextImg();
    TextImg::~TextImg();

    void TextImg::Build();

    float TextImg::Print(int x,int y, const char* str,...);

    void setSpacing(int value)
    { m_spacing = value; }

    int getSpacing()
    { return m_spacing; }

    int getSize()
    { return m_size; }

    int getCharShift()
    { return m_charshift; }

    void setCharShift(int value)
    { m_charshift = value; }

    void setFont(Texture fontimg)
    { m_font_image = fontimg; }

    protected:

	GLuint m_displaylist;
    Texture m_font_image;

    int
    m_spacing,
    m_charshift,
    m_size;
};

#endif
