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

    /// Construit un texture avec la valuer et la taille spécifier
    void build(Vector2i size, Vector4i color = 0, GLint internalFormat = 4, GLenum format = GL_RGBA);

    /// Chargement depuis un fichier
    void load(std::string filename, bool genMipMap = false, bool upperLeftOrigin = true);

    void operator()(std::string filename, bool genMipMap = false, bool upperLeftOrigin = true)
    {
        load(filename, genMipMap, upperLeftOrigin);
    }

    /// Operateur d'assignement pour chemin de fichier
    Texture & operator=(const Texture& copy);
    Texture & operator=(std::string texture);
    Texture & operator=(const char* texture);

    /// Verification de statue
    bool operator!() const;
    operator bool() const;

    /// Récuperation d'identifiant
    void setTextureName(GLuint textureName);
    GLuint getTextureName() const;
    operator GLuint() const;

    /// Taille de la texture
    void setSize(Vector2i size);
    virtual Vector2i getSize() const;

    /// Utilisation de la texture
    void use(bool state = true);

    /**
     * Supprime la texture si pas d'instance utiliser
     * note : plus sure que remove()
     */
    void release();

    /**
     * Supprime la texture directement
     * note : utiliser release() au lieu
     */
    void remove();

    /// Remplis la texture par la valeur color [0;255]
    void fill(Vector4i color);

    /// Spécifier le filtring appliquer a la texture
    void setFiltring(unsigned filtring);
    unsigned getFiltring() const;

    /**
     * Renvois true si la texture a été position de facon
     * a ce que les coordonnés 0,0 font référence au coin superieur a gauche
     * de la texture
     */
    bool isUpperLeftOrigin() const;

    /**
     * Renvois true si les couches du mipmap
     * ont été générer durant le chargement de l texture
     */
    bool isGenMipMap() const;

    /// Renvois le chemin d'accée a la texture par un fichier
    std::string getFilename() const;

    enum Filtring
    {
        NEAREST,
        LINEAR,
        MIPMAP,
    };

    static void resetCache();

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
