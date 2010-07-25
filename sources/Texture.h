#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "GLee.h"

#include <string>
#include <map>

#include "Mathematics.h"

namespace tbe
{

/// \brief Class de manipulation des texture utilise DevIL

class Texture
{
public:
    Texture();
    Texture(const Texture& copy);
    Texture(std::string filename, bool genMipMap = false, bool upperLeftOrigin = true);
    Texture(const char* filename, bool genMipMap = false, bool upperLeftOrigin = true);

    virtual ~Texture();

    /// Construit un texture avec la valuer et la taille sp�cifier
    void Build(Vector2i size, Vector4i color = 0, GLint internalFormat = 4, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);

    /// Chargement depuis un fichier
    void Load(std::string filename, bool genMipMap = false, bool upperLeftOrigin = true);

    void operator()(std::string filename, bool genMipMap = false, bool upperLeftOrigin = true)
    {
        Load(filename, genMipMap, upperLeftOrigin);
    }

    /// Operateur d'assignement pour chemin de fichier
    Texture & operator=(const Texture& copy);
    Texture & operator=(std::string texture);
    Texture & operator=(const char* texture);

    /// Verification de statue
    bool operator!() const;
    operator bool() const;

    /// R�cuperation d'identifiant
    void SetTextureName(GLuint textureName);
    GLuint GetTextureName() const;
    operator GLuint() const;

    /// Taille de la texture
    void SetSize(Vector2i size);
    virtual Vector2i GetSize() const;

    /// Utilisation de la texture
    void Use(bool state = true);

    /// Supprime la texture
    void Delete();

    /// Remplis la texture par la valeur color [0;255]
    void Fill(Vector4i color);

    /// Sp�cifier le filtring appliquer a la texture
    void SetFiltring(unsigned filtring);
    unsigned GetFiltring() const;

    /**
     * Renvois true si la texture a �t� position de facon
     * a ce que les coordonn�s 0,0 font r�f�rence au coin superieur a gauche
     * de la texture
     */
    bool IsUpperLeftOrigin() const;

    /**
     * Renvois true si les couches du mipmap
     * ont �t� g�n�rer durant le chargement de l texture
     */
    bool IsGenMipMap() const;

    /// Renvois le chemin d'acc�e a la texture par un fichier
    std::string GetFilename() const;

    enum Filtring
    {
        NEAREST,
        LINEAR,
        MIPMAP,
    };

    static void ResetCache();

    typedef std::map<unsigned, Texture> Map;

protected:
    std::string m_filename;
    GLuint m_textureName;
    Vector2i m_size;
    unsigned m_filtring;
    bool m_genMipMap;
    bool m_upperLeftOrigin;
};

}

#endif // _TEXTURE_H
