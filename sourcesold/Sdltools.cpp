#include <iostream>
#include <string>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <SDL.h>

#include "SDLTools.h"

using namespace std;

/*
GLuint glLoadTexture(string path, bool mipmap)
{
    int width, height;
    void* pixel;

    SDL_Surface* image = IMG_Load(path);

    SDL_PixelFormat format = *image->format;

    format.BitsPerPixel = 32;
    format.BytesPerPixel = 4;
    #if SDL_BYTEORDER == SDL_LIL_ENDIAN
    format.Rmask = 0x000000ff;
    format.Gmask = 0x0000ff00;
    format.Bmask = 0x00ff0000;
    format.Amask = 0xff000000;
    #else
    format.Rmask = 0xff000000;
    format.Gmask = 0x00ff0000;
    format.Bmask = 0x0000ff00;
    format.Amask = 0x000000ff;
    #endif

    SDL_Surface* converted = SDL_ConvertSurface(image,&format,SDL_HWSURFACE);

    width = converted->w;
    height = converted->h;
    pixel = converted->pixels;

    if(!image)
    throw string("Open Image File Error:\n")+path;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    if(mipmap)
    {
        gluBuild2DMipmaps (GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    }

    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    }

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    SDL_FreeSurface(converted);
    return texture;
}
*/
void glDrawSurface(SDL_Surface* surface, SDL_Rect rect)
{
    SDL_PixelFormat format = *surface->format;

    format.BitsPerPixel = 32;
    format.BytesPerPixel = 4;
    #if SDL_BYTEORDER == SDL_LIL_ENDIAN
    format.Rmask = 0x000000ff;
    format.Gmask = 0x0000ff00;
    format.Bmask = 0x00ff0000;
    format.Amask = 0xff000000;
    #else
    format.Rmask = 0xff000000;
    format.Gmask = 0x00ff0000;
    format.Bmask = 0x0000ff00;
    format.Amask = 0x000000ff;
    #endif

    SDL_Surface* converted = SDL_ConvertSurface(surface,&format,SDL_HWSURFACE);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, converted->w, converted->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, converted->pixels);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    rect.y = SDL_GetVideoSurface()->h-rect.y-rect.h;

    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2d(0,0);  glVertex2d(rect.x,rect.y+rect.h);
    glTexCoord2d(0,1);  glVertex2d(rect.x,rect.y);
    glTexCoord2d(1,0);  glVertex2d(rect.x+rect.w,rect.y+rect.h);
    glTexCoord2d(1,1);  glVertex2d(rect.x+rect.w,rect.y);
    glEnd();

    SDL_FreeSurface(converted);
    glDeleteTextures(1,&texture);
}

void glDrawSurface(SDL_Surface* surface, int x, int y, int w, int h)
{
    if(!w)w=surface->w;
    if(!h)h=surface->h;

    SDL_Rect rect = {x,y,w,h};
    glDrawSurface(surface,rect);
}
