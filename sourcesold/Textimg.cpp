#include "gl_textimg.h"

#define ImgCharSize 16
#define ImgSizeW 256
#define ImgSizeH 256

using namespace std;

TextImg::TextImg()
{
	m_displaylist = 0;

    m_spacing = 0;
    m_charshift = 0;
    m_size = 0;
}

TextImg::~TextImg()
{

}

float TextImg::Print(int x,int y, const char* str,...)
{
    float maxlength = 0;
    char display_str[1024];

    // Mise en format de la chaine avec les argument en plus

    va_list list;
    va_start(list,str);
        vsprintf(display_str,str,list);
    va_end(list);

    // Pour les text multi-line '\n'

    vector<char*> ptr(1,display_str);

    for(int i=0 ; display_str[i] != '\0' ; i++)
    if(display_str[i] == '\n')
    {
        ptr.push_back(&display_str[i+1]);
        display_str[i] = '\0';
    }

    // Rendue

    glPushMatrix();

        glLoadIdentity();
        glTranslatef(x,y,0);

        glListBase(m_displaylist+m_charshift-32);

        glBindTexture(GL_TEXTURE_2D, m_font_image);

        for(GLuint i=0 ; i<ptr.size() ; i++)
        {
            #define widthshift (float)((m_size+m_spacing)*strlen(ptr[i]))

            glCallLists(strlen(ptr[i]),GL_UNSIGNED_BYTE,ptr[i]);
            glTranslatef(-widthshift,-m_size,0);

            if(maxlength < widthshift)
            maxlength = widthshift;
        }

    glPopMatrix();

    return maxlength;
}

void TextImg::Build()
{
    #define calllistsNumber (ImgSizeW/ImgCharSize)*(ImgSizeH/ImgCharSize)

    // Creation des DisplayLists

    m_displaylist = glGenLists(calllistsNumber);

    for(int i=0 ; i<calllistsNumber ; i++)
    {
        float x = float (i%ImgCharSize) / (ImgSizeW/ImgCharSize);
		float y = float (i/ImgCharSize) / (ImgSizeH/ImgCharSize);

        glNewList(m_displaylist+i,GL_COMPILE);

            #define decalex (float)(1.0f/(ImgSizeW/ImgCharSize))
            #define decaley (float)(1.0f/(ImgSizeH/ImgCharSize))

            glBindTexture(GL_TEXTURE_2D,m_font_image);

            glBegin(GL_QUADS);
                glTexCoord2f(x,             1.0f + y + decaley  );  glVertex2i(0,0);
                glTexCoord2f(x + decalex,   1.0f + y + decaley  );  glVertex2i(m_size,0);
                glTexCoord2f(x + decalex,   1.0f + y            );  glVertex2i(m_size,m_size);
                glTexCoord2f(x,             1.0f + y            );  glVertex2i(0,m_size);
            glEnd();

			glTranslated(m_size+m_spacing,0,0);

        glEndList();
    }
}
