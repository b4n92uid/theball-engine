/*
 * File:   Pencil.cpp
 * Author: b4n92uid
 *
 * Created on 11 mai 2010, 19:12
 */

#include "Pencil.h"
#include "Skin.h"
#include "Texture.h"
#include "Tools.h"

#include <fstream>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#define PENCIL_CHAR_COUNT (256)

using namespace std;
using namespace tbe;
using namespace tbe::gui;

class InternalFreetypeFaceManager
{
public:

    typedef map<Pencil*, string> InstanceMap;

    InternalFreetypeFaceManager()
    {
        FT_Init_FreeType(&library);
    }

    ~InternalFreetypeFaceManager()
    {
        FT_Done_FreeType(library);

        for(InstanceMap::iterator itt = instances.begin(); itt != instances.end(); itt++)
            itt->first->remove();
    }

    FT_Library library;
    InstanceMap instances;
};

static InternalFreetypeFaceManager manager;

Pencil::Pencil()
{
    m_color = 1;
    m_lineSpace = 10;
    m_tabSize = 4;
    m_fontSize = 0;
    m_charList = 0;
}

Pencil::Pencil(std::string font, int size)
{
    m_color = 1;
    m_lineSpace = 10;
    m_tabSize = 4;
    m_fontSize = 0;
    m_charList = 0;

    load(font, size);
}

Pencil::~Pencil()
{
    bool deleteThis = true;

    for(InternalFreetypeFaceManager::InstanceMap::iterator i = manager.instances.begin(); i != manager.instances.end(); i++)
        if(i->first->m_charList == m_charList)
            deleteThis = false;

    if(deleteThis)
        remove();

    manager.instances.erase(this);
}

void Pencil::remove()
{
    if(m_charList)
        glDeleteLists(m_charList, 1);
}

std::string Pencil::wrapeLine(std::string str, float width, bool takeEnd) const
{
    float curWidth = 0;

    switch(takeEnd)
    {
        case true:
            for(unsigned j = str.size() - 1; j >= 0; j--)
            {
                curWidth += m_charsWidth[str[j]];

                if(curWidth >= width)
                    return str.substr(j + 1);
            }
            break;

        case false:
            for(unsigned j = 0; j < str.size(); j++)
            {
                curWidth += m_charsWidth[str[j]];

                if(curWidth >= width)
                    return str.substr(0, j - 1);
            }
            break;
    }

    return str;
}

std::vector<std::string> Pencil::getWrapedLines(std::string str, float width) const
{
    std::vector<std::string> widthOverflow;

    float curWidth = 0;
    unsigned offset = 0;

    for(unsigned i = 0; i < str.size(); i++)
    {
        unsigned char c = str[i];

        curWidth += m_charsWidth[c < (int)m_charsWidth.size() ? c : 32];

        if(curWidth >= width)
        {
            widthOverflow.push_back(str.substr(offset, i - offset));

            offset = i;
            curWidth = 0;
        }
    }

    widthOverflow.push_back(str.substr(offset));

    return widthOverflow;
}

Vector2f Pencil::centerOf(GuiString text, Vector2f pos, Vector2f size) const
{
    Vector2f textSize = sizeOf(text);

    Vector2f textPos(
                     pos.x + size.x / 2.0f - textSize.x / 2.0f,
                     pos.y + size.y / 2.0f - textSize.y / 2.0f
                     );

    return textPos;
}

Vector2f Pencil::centerOf(std::string str, Vector2f pos, Vector2f size) const
{
    Vector2f textSize = sizeOf(str);

    Vector2f textPos(
                     pos.x + size.x / 2.0f - textSize.x / 2.0f,
                     pos.y + size.y / 2.0f - textSize.y / 2.0f
                     );

    return textPos;
}

Vector2f Pencil::sizeOf(GuiString text) const
{
    Vector2f returnSize;
    float maxWidth = 0;

    for(unsigned i = 0; i < text.size(); i++)
    {
        Vector2f size = sizeOf(text[i]);
        maxWidth = max(size.x, maxWidth);
        returnSize.y += m_fontSize + m_lineSpace;
    }

    returnSize.x = maxWidth;
    returnSize.y -= m_lineSpace;

    return returnSize;
}

Vector2f Pencil::sizeOf(std::string str) const
{
    Vector2f size(0, m_fontSize);

    for(unsigned i = 0; i < str.size(); i++)
    {
        unsigned char c = str[i];

        if(c < (int)m_charsWidth.size())
            size.x += m_charsWidth[c];
        else
            size.x += m_charsWidth[32];
    }

    return size;
}

void Pencil::display(Vector2f pos, GuiString text)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4fv(m_color);

    glListBase(m_charList);

    // float modelview_matrix[16];
    // glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

    // This Is Where The Text Display Actually Happens.
    // For Each Line Of Text We Reset The Modelview Matrix
    // So That The Line's Text Will Start In The Correct Position.
    // Notice That We Need To Reset The Matrix, Rather Than Just Translating
    // Down By h. This Is Because When Each Character Is
    // Drawn It Modifies The Current Matrix So That The Next Character
    // Will Be Drawn Immediately After It.

    glPushMatrix();

    for(unsigned i = 0; i < text.size(); i++)
    {
        glLoadIdentity();
        glTranslatef(pos.x, pos.y, 0);

        // The Commented Out Raster Position Stuff Can Be Useful If You Need To
        // Know The Length Of The Text That You Are Creating.
        // If You Decide To Use It Make Sure To Also Uncomment The glBitmap Command
        // In make_dlist().

        // glRasterPos2f(0, 0);

        glCallLists(text[i].length(), GL_UNSIGNED_BYTE, text[i].c_str());

        // float rpos[4];
        // glGetFloatv(GL_CURRENT_RASTER_POSITION, rpos);

        // (Assuming No Rotations Have Happend)
        // float curwidth = rpos[0] - x;

        pos.y += m_fontSize + m_lineSpace;

    }

    glPopMatrix();

    glColor4fv(Vector4f(1));
}

void Pencil::load(std::string path, int size)
{
    vector<unsigned> charindex;

    charindex.resize(PENCIL_CHAR_COUNT);

    for(unsigned i = 0; i < PENCIL_CHAR_COUNT; i++)
        charindex[i] = i;

    m_charsWidth.resize(charindex.size());

    // The Node In Which FreeType Holds Information On A Given
    // Font Is Called A "face".
    FT_Face face;

    // This Is Where We Load In The Font Information From The File.
    // Of All The Places Where The Code Might Die, This Is The Most Likely,
    // As FT_New_Face Will Fail If The Font File Does Not Exist Or Is Somehow Broken.
    if(FT_New_Face(manager.library, path.c_str(), 0, &face))
        throw Exception("Pencil::Load; Open font file error (%s)", path.c_str());

    cout << "Load font file : " << path << endl;

    // For Some Twisted Reason, FreeType Measures Font Size
    // In Terms Of 1/64ths Of Pixels. Thus, To Make A Font
    // h Pixels High, We Need To Request A Size Of h*64.
    // (h << 6 Is Just A Prettier Way Of Writing h*64)
    FT_Set_Char_Size(face, size << 6, size << 6, 96, 96);

    // Here We Ask OpenGL To Allocate Resources For
    // All The Textures And Display Lists Which We
    // Are About To Create.
    m_charList = glGenLists(charindex.size());
    GLuint textures[charindex.size()];
    glGenTextures(charindex.size(), textures);

    // This Is Where We Actually Create Each Of The Fonts Display Lists.
    for(unsigned i = 0; i < charindex.size(); i++)
    {
        // The First Thing We Do Is Get FreeType To Render Our Character
        // Into A Bitmap. This Actually Requires A Couple Of FreeType Commands:
        // Load The Glyph For Our Character.

        if(FT_Load_Glyph(face, FT_Get_Char_Index(face, charindex[i]), FT_LOAD_DEFAULT))
            throw Exception("Pencil::Load; FT_Load_Glyph failed");

        // Move The Face's Glyph Into A Glyph Node.
        FT_Glyph glyph;
        if(FT_Get_Glyph(face->glyph, &glyph))
            throw Exception("Pencil::Load; FT_Get_Glyph failed");

        // Convert The Glyph To A Bitmap.
        FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
        FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

        // This Reference Will Make Accèssing The Bitmap Easier.
        FT_Bitmap & bitmap = bitmap_glyph->bitmap;

        // Use Our Helper Function To Get The Widths Of
        // The Bitmap Data That We Will Need In Order To Create
        // Our Texture.
        int width = tools::nextPow2(bitmap.width);
        int height = tools::nextPow2(bitmap.rows);

        //m_charsWidth[i] = width;

        // Allocate Memory For The Texture Data.
        GLubyte * expanded_data = new GLubyte[ 2 * width * height];

        // Here We Fill In The Data For The Expanded Bitmap.
        // Notice That We Are Using A Two Channel Bitmap (One For
        // Channel Luminosity And One For Alpha), But We Assign
        // Both Luminosity And Alpha To The Value That We
        // Find In The FreeType Bitmap.
        // We Use The ?: Operator To Say That Value Which We Use
        // Will Be 0 If We Are In The Padding Zone, And Whatever
        // Is The FreeType Bitmap Otherwise.
        for(int j = 0; j < height; j++)
        {
            for(int i = 0; i < width; i++)
            {
                expanded_data[2 * (i + j * width)] = 255;
                expanded_data[2 * (i + j * width) + 1] =
                        (i >= bitmap.width || j >= bitmap.rows) ? 0 : bitmap.buffer[i + bitmap.width * j];
            }
        }

        // Now We Just Setup Some Texture Parameters.
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // Here We Actually Create The Texture Itself, Notice
        // That We Are Using GL_LUMINANCE_ALPHA To Indicate That
        // We Are Using 2 Channel Data.
        glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data);

        // With The Texture Created, We Don't Need The Expanded Data Anymore.
        delete [] expanded_data;

        // Now We Create The Display List
        glNewList(m_charList + i, GL_COMPILE);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glPushMatrix();

        // First We Need To Move Over A Little So That
        // The Character Has The Right Amount Of Space
        // Between It And The One Before It.
        glTranslatef(bitmap_glyph->left, 0, 0);

        // Now We Move Down A Little In The Case That The
        // Bitmap Extends Past The Bottom Of The Line
        // This Is Only True For Characters Like 'g' Or 'y'.
        glTranslatef(0, bitmap_glyph->top - bitmap.rows, 0);

        // Now We Need To Account For The Fact That Many Of
        // Our Textures Are Filled With Empty Padding Space.
        // We Figure What Portion Of The Texture Is Used By
        // The Actual Character And Store That Information In
        // The x And y Variables, Then When We Draw The
        // Quad, We Will Only Reference The Parts Of The Texture
        // That Contains The Character Itself.
        float x = (float)bitmap.width / (float)width,
                y = (float)bitmap.rows / (float)height;

        // Here We Draw The Texturemapped Quads.
        // The Bitmap That We Got From FreeType Was Not
        // Oriented Quite Like We Would Like It To Be,
        // But We Link The Texture To The Quad
        // In Such A Way That The Result Will Be Properly Aligned.
        glBegin(GL_QUADS);
        glTexCoord2d(0, 0);
        glVertex2f(0, bitmap.rows);
        glTexCoord2d(0, y);
        glVertex2f(0, 0);
        glTexCoord2d(x, y);
        glVertex2f(bitmap.width, 0);
        glTexCoord2d(x, 0);
        glVertex2f(bitmap.width, bitmap.rows);
        glEnd();

        glPopMatrix();

        glTranslatef(face->glyph->advance.x >> 6, 0, 0);

        m_charsWidth[i] = face->glyph->advance.x >> 6;

        // Increment The Raster Position As If We Were A Bitmap Font.
        // (Only Needed If You Want To Calculate Text Length)
        // glBitmap(0, 0, 0, 0, face->glyph->advance.x >> 6, 0, NULL);

        // Finish The Display List
        glEndList();
    }

    m_fontSize = size;
    m_fontPath = path;

    // We Don't Need The Face Information Now That The Display
    // Lists Have Been Created, So We Free The Assosiated Resources.
    FT_Done_Face(face);

    manager.instances[this] = path;
}

bool Pencil::isReady()
{
    return m_charList && m_fontSize && !m_fontPath.empty();
}

Pencil::operator bool()
{
    return isReady();
}

bool Pencil::operator!()
{
    return !isReady();
}

void Pencil::setFontSize(int fontsize)
{
    this->m_fontSize = fontsize;
    load(m_fontPath, m_fontSize);
}

int Pencil::getFontSize() const
{
    return m_fontSize;
}

void Pencil::setTabSize(int tabSize)
{
    this->m_tabSize = tabSize;
}

int Pencil::getTabSize() const
{
    return m_tabSize;
}

void Pencil::setLineSpace(int lineSpace)
{
    this->m_lineSpace = lineSpace;
}

int Pencil::getLineSpace() const
{
    return m_lineSpace;
}

void Pencil::setColor(Vector4f color)
{
    this->m_color = color;
}

Vector4f Pencil::getColor() const
{
    return m_color;
}

// Text ------------------------------------------------------------------------

GuiString::GuiString()
{
}

GuiString::GuiString(const char* str, ...)
{
    char* output = new char[strlen(str) + 1024];

    va_list list;
    va_start(list, str);
    vsprintf(output, str, list);
    va_end(list);

    format(output);

    delete output;
}

GuiString::GuiString(std::string str, ...)
{
    char* output = new char[str.size() + 1024];

    va_list list;
    va_start(list, str);
    vsprintf(output, str.c_str(), list);
    va_end(list);

    format(output);

    delete output;
}

void GuiString::format(std::string str)
{
    unsigned offset = 0;

    for(unsigned i = 0; str[i] != '\0'; i++)
    {
        if(str[i] == '\n')
        {
            string curLine(str, offset, i - offset);

            push_back(curLine);

            offset = i + 1;
        }
    }

    push_back(string(str, offset, string::npos));
}

void GuiString::loadFromFile(std::string filepath)
{
    ifstream file(filepath.c_str());

    if(!file)
        throw Exception("Text::LoadFromFile; Open file error (%s)", filepath.c_str());

    stringstream ss;
    ss << file.rdbuf();

    format(ss.str());

    file.close();
}
