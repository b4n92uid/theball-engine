#ifndef TBE_H
#define TBE_H

#include <iostream>

#include "GLee.h"
#include <GL/glu.h>

#include "Mathematics.h"
#include "Clock.h"

namespace tbe
{

class EventManager;

namespace ppe
{
class PostProcessManager;
}

namespace scene
{
class SceneManager;
}

namespace ticks
{
class FpsManager;
}

/**
 * \brief Class d'interface générale du moteur
 *
 * Inclus:
 *  un gestionnaire de Scene
 *  un gestionnaire de FPS
 *  un Recepteur d'evenement de la fenetre (EventRecv)
 **/

class Device
{
public:

    /// Constructeur
    Device();

    /// Destructeur
    virtual ~Device();

    /**
     * Initilisation, cette fonction doit etre appeller avant
     * tout interaction avec l'objet
     */
    void init();

    /// Efface la scene, et commence une nouvelle frame
    void beginScene();

    /// Termine et rend la scene
    void endScene();

    /// Accès au gestionnaire d'évenement
    EventManager* getEventManager();

    /// Accès aux gestionnaire de Scene
    scene::SceneManager* getSceneManager();

    /// Accès aux gestionnaire du Fps
    ticks::FpsManager* getFpsManager();

    /// Accès aux gestionnaire d'effets apres traitement
    ppe::PostProcessManager* getPostProcessManager() const;

    /// Spécifier la taille du viewport
    void setViewportSize(Vector2i viewportSize);

    /// Renvois la taille du viewport
    Vector2i getViewportSize() const;

    void setClearColor(Vector4f clearColor);
    Vector4f getClearColor() const;

    /// Renvoi la version du moteur (Build Date)
    static std::string getVersion();

protected:
    scene::SceneManager* m_sceneManager;
    ppe::PostProcessManager* m_postProcessManager;
    ticks::FpsManager* m_fpsManager;
    EventManager* m_eventManager;

    ticks::Clock m_timestamp;

    Vector2i m_viewportSize;
    Vector4f m_clearColor;
};


}

#endif // TBE_H
