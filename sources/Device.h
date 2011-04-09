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

namespace gui
{
class GuiManager;
}

namespace ticks
{
class FpsManager;
}

/**
 * \brief Class d'interface generale du moteur
 *
 * Inclus:
 * un gestionnaire de GUI
 * un gestionnaire de Scene
 * un gestionnaire de FPS
 * un Recepteur d'evenement de la fenetre (EventRecv)
 * autre fonction lier a la fenetre
 **/

class Device
{
public:

    /// Constructeur
    Device();

    /// Destructeur
    virtual ~Device();

    /// Efface la scene
    void beginScene();

    /// Rendue de la scene
    void endScene();

    /// Accès au evenement récpere
    EventManager* getEventManager();

    /// Accès aux gestionnaire de SCENE
    scene::SceneManager* getSceneManager();

    /// Accès aux gestionnaire de GUI
    gui::GuiManager* getGuiManager();

    /// Accès aux gestionnaire du FPS
    ticks::FpsManager* getFpsManager();

    /// Accès aux gestionnaire d'effets apres traitement
    ppe::PostProcessManager* getPostProcessManager() const;

    /// Initilisation
    void init();

    /// Taille du viewport
    void setViewportSize(Vector2i viewportSize);
    Vector2i getViewportSize() const;

    // Static ------------------------------------------------------------------

    /// Renvoi la version du moteur (Build Date)
    static std::string getVersion();

protected:
    scene::SceneManager* m_sceneManager;
    ppe::PostProcessManager* m_postProcessManager;
    ticks::FpsManager* m_fpsManager;
    gui::GuiManager* m_guiManager;
    EventManager* m_eventManager;
    
    ticks::Clock m_timestamp;

    Vector2i m_viewportSize;
};


}

#endif // TBE_H
