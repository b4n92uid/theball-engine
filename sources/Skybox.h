#ifndef _SKYBOX_H
#define _SKYBOX_H

#include "Mathematics.h"
#include "Texture.h"

namespace tbe
{
namespace scene
{

/**
 * \brief Class de représentation d'un Skybox
 * Ordre des texture : Devant, Derrier, Haut, Bas, Gauche, Droit
 */
class SkyBox
{
public:
    SkyBox();
    SkyBox(Texture textures[]);
    ~SkyBox();

    /// Tableaux de 6 texture pour l'affichage
    void setTextures(Texture textures[]);
    Texture* getTextures();

    /// Réinitialisation
    void clear();
    
    /// Rendue du skybox
    /// \param camerapos Position de la camera
    void render(const Vector3f& camerapos);

    void setEnable(bool enable);
    bool isEnable() const;

protected:
    void initRender();

protected:
    GLuint m_renderID;
    Texture m_textures[6];
    bool m_enable;
};

}
}

#endif // _SKYBOX_H
