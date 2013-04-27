#include "Texture.h"

#include <algorithm>
#include <iostream>

#include <GL/glu.h>

#include <IL/il.h>

#include "Exception.h"
#include "Tools.h"

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
        Free();
    }

    void Reload()
    {
        for(iterator itt = begin(); itt != end(); itt++)
        {
            string filename = itt->first->getFilename();
            bool mipmap = itt->first->isGenMipMap();
            bool ulorig = itt->first->isUpperLeftOrigin();

            itt->first->remove();
            itt->first->load(filename, mipmap, ulorig);
        }
    }

    void Free()
    {
        for(iterator itt = begin(); itt != end(); itt++)
            itt->first->remove();

        clear();
    }

    Texture* IsExist(GLuint id)
    {
        for(iterator itt = begin(); itt != end(); itt++)
            if(itt->first->getTextureName() == id)
                return itt->first;

        return NULL;
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
    m_persistent = false;
    m_genMipMap = false;
    m_upperLeftOrigin = false;
    m_filtring = 0;
    m_anistropy = 1;
}

Texture::Texture(const Texture& copy)
{
    m_textureName = 0;
    m_persistent = false;
    m_genMipMap = false;
    m_upperLeftOrigin = false;
    m_filtring = 0;
    m_anistropy = 1;

    *this = copy;
}

Texture::Texture(std::string filename, bool genMipMap, bool upperLeftOrigin)
{
    m_textureName = 0;
    m_persistent = false;
    m_genMipMap = false;
    m_upperLeftOrigin = false;
    m_filtring = 0;
    m_anistropy = 1;

    load(filename, genMipMap, upperLeftOrigin);
}

Texture::Texture(const char* filename, bool genMipMap, bool upperLeftOrigin)
{
    m_textureName = 0;
    m_persistent = false;
    m_genMipMap = false;
    m_upperLeftOrigin = false;
    m_filtring = 0;
    m_anistropy = 1;

    load(filename, genMipMap, upperLeftOrigin);
}

Texture::~Texture()
{
    release();
}

Texture& Texture::operator =(const Texture& copy)
{
    if(!copy.m_filename.empty())
        manager[this] = m_filename = copy.m_filename;

    m_textureName = copy.m_textureName;
    m_persistent = copy.m_persistent;
    m_genMipMap = copy.m_genMipMap;
    m_upperLeftOrigin = copy.m_upperLeftOrigin;
    m_filtring = copy.m_filtring;
    m_anistropy = copy.m_anistropy;

    m_size = copy.m_size;

    return *this;
}

Texture& Texture::operator =(const char* filename)
{
    load(filename);

    return *this;
}

Texture& Texture::operator =(std::string filename)
{
    load(filename);

    return *this;
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

void Texture::remove()
{
    if(m_textureName)
    {
        glDeleteTextures(1, &m_textureName);

        m_textureName = 0;
        m_persistent = false;
        m_genMipMap = false;
        m_upperLeftOrigin = false;
        m_filtring = 0;
        m_anistropy = 1;

        m_filename.clear();
        m_size = 0;
    }
}

void Texture::release()
{
    if(manager.count(this))
    {
        manager.erase(this);

        if(!m_persistent && !manager.IsExist(m_filename))
            remove();
    }

    m_textureName = 0;
    m_persistent = false;
    m_genMipMap = false;
    m_upperLeftOrigin = false;
    m_filtring = 0;
    m_anistropy = 1;

    m_filename.clear();
    m_size = 0;
}

void Texture::load(std::string filename, bool genMipMap, bool upperLeftOrigin)
{
    Texture* sharedTexture = manager.IsExist(filename);

    if(sharedTexture)
    {
        *this = *sharedTexture;
        return;
    }

    // DevIL -------------------------------------------------------------------

    cout << "[Texture] " << filename << endl;

    if(upperLeftOrigin)
    {
        ilEnable(IL_ORIGIN_SET);
        ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
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

    if(!math::isPow2(m_size.x) || !math::isPow2(m_size.y))
        cout << "***WARNING*** Texture::Load; Texture is not pow2 dim (" << filename << ")" << endl;

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
        setFiltring(MIPMAP | LINEAR);
    }

    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, 4, m_size.x, m_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        setFiltring(LINEAR);
    }

    // -------------------------------------------------------------------------

    ilDeleteImage(id);

    delete[] pixels;

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::build(Vector2i size, Vector4i color, GLint internalFormat, GLenum format, GLenum type)
{
    if(!math::isPow2(size.x) || !math::isPow2(size.y))
        cout << "***WARNING*** Texture::Build; Texture is not pow2 dim " << size << endl;

    unsigned byteSize = size.x * size.y * 4;
    GLubyte* pixels = new GLubyte[byteSize];

    for(unsigned i = 0; i < byteSize; i += 4)
    {
        pixels[i + 0] = color.x;
        pixels[i + 1] = color.y;
        pixels[i + 2] = color.z;
        pixels[i + 3] = color.w;
    }

    buildMem(size, pixels, internalFormat, format, type);
    delete[] pixels;
}

void Texture::buildMem(Vector2i size, unsigned char* byte, GLint internalFormat, GLenum format, GLenum type)
{
    if(!math::isPow2(size.x) || !math::isPow2(size.y))
        cout << "***WARNING*** Texture::Build; Texture is not pow2 dim " << size << endl;

    m_size = size;

    glGenTextures(1, &m_textureName);
    glBindTexture(GL_TEXTURE_2D, m_textureName);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_size.x, m_size.y, 0, format, type, byte);
    setFiltring(LINEAR);

    m_persistent = false;
    m_genMipMap = false;
    m_upperLeftOrigin = false;

    m_filtring = 0;
    m_anistropy = 1;

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::fill(Vector4i color)
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

void Texture::use(bool state)
{
    if(state && m_textureName)
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

Vector2i Texture::getSize() const
{
    return m_size;
}

GLuint Texture::getTextureName() const
{
    return m_textureName;
}

void Texture::setFiltring(unsigned filtring)
{
    this->m_filtring = filtring;

    use(true);

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

    use(false);
}

unsigned Texture::getFiltring() const
{
    return m_filtring;
}

void Texture::setAnistropy(unsigned anistropy)
{
    this->m_anistropy = std::max(std::min(anistropy, 8u), 1u);

    use(true);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_anistropy);
    use(false);
}

unsigned Texture::getAnistropy() const
{
    return m_anistropy;
}

bool Texture::isUpperLeftOrigin() const
{
    return m_upperLeftOrigin;
}

bool Texture::isGenMipMap() const
{
    return m_genMipMap;
}

std::string Texture::getFilename() const
{
    return m_filename;
}

void Texture::setPersistent(bool persistent)
{
    this->m_persistent = persistent;
}

bool Texture::isPersistent() const
{
    return m_persistent;
}

void Texture::resetCache()
{
    manager.Free();
}

void Texture::reloadCache()
{
    manager.Reload();
}

Texture* Texture::fetch(GLuint id)
{
    return manager.IsExist(id);
}
