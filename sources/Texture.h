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

    /// Construit une texture avec la valuer et la taille spécifier
    void build(Vector2i size, Vector4i color = 0, GLint internalFormat = 4, GLenum format = GL_RGBA);

    /// Construit une texture avec la valuer et la taille spécifier
    void buildMem(Vector2i size, unsigned char* byte, GLint internalFormat = 4, GLenum format = GL_RGBA);

    /**
     * Chargement la texture depuis un fichier
     *
     * @param filename Chemin vers la texture
     * @param genMipMap Définie si le chargement de la texture générera un mipMapping (texture a plusieur niveau de détail)
     * @param upperLeftOrigin Définie si la texture devera etre retourné pour que (0,0) correspend au coin du haut a gauche
     */
    void load(std::string filename, bool genMipMap = false, bool upperLeftOrigin = true);

    void loadMask(std::string filename, bool upperLeftOrigin = true);

    /// @see load()

    void operator()(std::string filename, bool genMipMap = false, bool upperLeftOrigin = true)
    {
        load(filename, genMipMap, upperLeftOrigin);
    }

    /// Operateur d'assginement : charge la texture spécifier par filename
    Texture & operator=(std::string filename);

    /// Operateur d'assginement : charge la texture spécifier par filename
    Texture & operator=(const char* filename);

    /// Operateur de copie
    Texture & operator=(const Texture& copy);

    /// Verification de statue, renvois true si une texture est bien charger
    bool operator!() const;

    /// Verification de statue, renvois true si une texture est bien charger
    operator bool() const;

    /// Récuperation d'identifiant OpenGL
    GLuint getTextureName() const;

    /// @see getTextureName()
    operator GLuint() const;

    /// Renvois la taille de la texture
    virtual Vector2i getSize() const;

    /// Utilisation de la texture (bind)
    void use(bool state = true);

    /**
     * Supprime la texture si pas d'instance utiliser
     * note : plus sure que remove()
     * @see remove()
     */
    void release();

    /**
     * Supprime la texture directement
     * note : utiliser release() au lieu
     * @see release()
     */
    void remove();

    /// Remplis la texture par la valeur color [0;255]
    void fill(Vector4i color);

    enum Filtring
    {
        NEAREST = 2,
        LINEAR = 4,
        MIPMAP = 8,
    };

    /// Spécifier le filtring appliquer a la texture
    void setFiltring(unsigned filtring);

    /// Renvois le filtring utliser par la texture
    unsigned getFiltring() const;

    void setAnistropy(unsigned anistropy);
    unsigned getAnistropy() const;

    /**
     * Renvois true si la texture a été position de facon
     * a ce que les coordonnés 0,0 font référence au coin
     * superieur a gauche de la texture
     */
    bool isUpperLeftOrigin() const;

    /**
     * Renvois true si les couches du mipmap
     * ont été générer durant le chargement de la texture
     */
    bool isGenMipMap() const;

    /// Renvois le chemin d'accée a la texture par un fichier
    std::string getFilename() const;

    void setPersistent(bool persistent);
    bool isPersistent() const;

    /**
     * Supprime toute les texture partager
     * attention : tout les texture déja charger devienderont invalide
     */
    static void resetCache();
    
    static Texture* fetch(GLuint id);

    typedef std::map<unsigned, Texture> Map;

protected:

    std::string m_filename;

    GLuint m_textureName;

    Vector2i m_size;

    bool m_persistent;
    bool m_genMipMap;
    bool m_upperLeftOrigin;

    unsigned m_filtring;
    unsigned m_anistropy;
};

}

#endif // _TEXTURE_H
