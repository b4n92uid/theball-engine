#include "Texture.h"

#include <algorithm>
#include <iostream>

#include <GL/glu.h>

#include <IL/il.h>

#include "Exception.h"

using namespace std;
using namespace tbe;

class SharedTextureManager : public map<Texture*, string>
{
public:

    SharedTextureManager()
    {
        ilInit();
    }

    ~SharedTextureManager()
    {
        for(iterator itt = begin(); itt != end(); itt++)
        {
            cout << "MANAGER Free texture : " << itt->first->GetFilename() << endl;
            itt->first->Delete();
        }

        clear();
    }

    Texture* IsExist(string path)
    {
        for(iterator itt = begin(); itt != end(); itt++)
            if(itt->second == path)
                return itt->first;

        return NULL;
    }
};

static SharedTextureManager manager;

Texture::Texture()
{
    m_textureName = 0;
    m_filtring = 0;
    m_genMipMap = false;
    m_upperLeftOrigin = false;
}

Texture::Texture(const Texture& copy)
{
    *this = copy;
}

Texture::Texture(std::string filename, bool genMipMap, bool upperLeftOrigin)
{
    Load(filename, genMipMap, upperLeftOrigin);
}

Texture::Texture(const char* filename, bool genMipMap, bool upperLeftOrigin)
{
    Load(filename, genMipMap, upperLeftOrigin);
}

Texture::~Texture()
{
    if(manager.count(this))
    {
        manager.erase(this);

        if(!manager.IsExist(m_filename))
        {
            cout << "DTOR Free texture : " << m_filename << endl;
            Delete();
        }
    }
}

Texture& Texture::operator =(const Texture& copy)
{
    if(!copy.m_filename.empty())
        manager[this] = m_filename = copy.m_filename;

    m_filtring = copy.m_filtring;
    m_genMipMap = copy.m_genMipMap;
    m_size = copy.m_size;
    m_textureName = copy.m_textureName;
    m_upperLeftOrigin = copy.m_upperLeftOrigin;

    return *this;
}

Texture& Texture::operator =(const char* texture)
{
    Load(texture);

    return *this;
}

Texture& Texture::operator =(std::string texture)
{
    Load(texture);

    return *this;
}

void Texture::Load(std::string filename, bool genMipMap, bool upperLeftOrigin)
{
    Texture* sharedTexture = manager.IsExist(filename);

    if(sharedTexture)
    {
        cout << "Load shared texture : " << sharedTexture->m_filename << endl;
        *this = *sharedTexture;
        return;
    }

    // DevIL -------------------------------------------------------------------

    cout << "Load texture file : " << filename << endl;

    if(upperLeftOrigin)
    {
        ilEnable(IL_ORIGIN_SET);
        ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
    }

    else
    {
        ilEnable(IL_ORIGIN_SET);
        ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    }

    ilEnable(IL_FORMAT_SET);
    ilFormatFunc(IL_RGBA);

    ilEnable(IL_TYPE_SET);
    ilTypeFunc(IL_UNSIGNED_BYTE);


    ILuint id = ilGenImage();
    ilBindImage(id);

    ILboolean error = !ilLoadImage(filename.c_str());

    if(error)
        throw Exception("Texture::Load; Open file error (%s)", filename.c_str());

    manager[this] = m_filename = filename;

    m_size.x = ilGetInteger(IL_IMAGE_WIDTH);
    m_size.y = ilGetInteger(IL_IMAGE_HEIGHT);

    m_genMipMap = genMipMap;
    m_upperLeftOrigin = upperLeftOrigin;

    ILubyte* pixels = new ILubyte[m_size.x * m_size.y * 4];

    ilCopyPixels(0, 0, 0, m_size.x, m_size.y, 1, IL_RGBA, IL_UNSIGNED_BYTE, pixels);

    // OpenGL ------------------------------------------------------------------

    glGenTextures(1, &m_textureName);
    glBindTexture(GL_TEXTURE_2D, m_textureName);

    if(genMipMap)
    {
        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, m_size.x, m_size.y, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        SetFiltring(MIPMAP | LINEAR);
    }

    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, 4, m_size.x, m_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        SetFiltring(LINEAR);
    }

    // -------------------------------------------------------------------------

    ilDeleteImage(id);

    delete[] pixels;

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Fill(Vector4i color)
{
    unsigned byteSize = m_size.x * m_size.y * 4;

    GLubyte* pixels = new GLubyte[byteSize];

    for(unsigned i = 0; i < byteSize; i += 4)
    {
        pixels[i + 0] = color.x;
        pixels[i + 1] = color.y;
        pixels[i + 2] = color.z;
        pixels[i + 3] = color.w;
    }

    glBindTexture(GL_TEXTURE_2D, m_textureName);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, m_size.x, m_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Build(Vector2i size, Vector4i color, GLint internalFormat, GLenum format, GLenum type)
{
    m_size = size;

    glGenTextures(1, &m_textureName);
    glBindTexture(GL_TEXTURE_2D, m_textureName);

    unsigned byteSize = m_size.x * m_size.y * 4;

    GLubyte* pixels = new GLubyte[byteSize];

    for(unsigned i = 0; i < byteSize; i += 4)
    {
        pixels[i + 0] = color.x;
        pixels[i + 1] = color.y;
        pixels[i + 2] = color.z;
        pixels[i + 3] = color.w;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_size.x, m_size.y, 0, format, type, pixels);

    SetFiltring(Texture::LINEAR);

    delete[] pixels;

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete()
{
    if(m_textureName)
        glDeleteTextures(1, &m_textureName);
}

void Texture::Use(bool state)
{
    if(state)
    {
        GLuint binded;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*) & binded);

        if(binded != m_textureName)
            glBindTexture(GL_TEXTURE_2D, m_textureName);
    }

    else
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Texture::SetSize(Vector2i size)
{
    this->m_size = size;
}

Vector2i Texture::GetSize() const
{
    return m_size;
}

void Texture::SetTextureName(GLuint textureName)
{
    this->m_textureName = textureName;
}

GLuint Texture::GetTextureName() const
{
    return m_textureName;
}

void Texture::SetFiltring(unsigned filtring)
{
    this->m_filtring = filtring;

    Use(true);

    if(m_filtring & NEAREST)
    {
        if(m_filtring & MIPMAP)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

        else
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    else if(m_filtring & LINEAR)
    {
        if(m_filtring & MIPMAP)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        else
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    Use(false);
}

unsigned Texture::GetFiltring() const
{
    return m_filtring;
}

bool Texture::IsUpperLeftOrigin() const
{
    return m_upperLeftOrigin;
}

bool Texture::IsGenMipMap() const
{
    return m_genMipMap;
}

std::string Texture::GetFilename() const
{
    return m_filename;
}

bool Texture::operator!() const
{
    return !m_textureName;
}

Texture::operator bool() const
{
    return m_textureName;
}

Texture::operator GLuint() const
{
    return m_textureName;
}
