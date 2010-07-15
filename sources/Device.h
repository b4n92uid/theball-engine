#ifndef TBE_H
#define TBE_H

#include <iostream>
#include <io.h>

#include "GLee.h"
#include <GL/glu.h>

#include "Mathematics.h"

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
    void BeginScene();

    /// Rendue de la scene
    void EndScene();

    /// Accès au evenement récpere
    EventManager* GetEventManager();

    /// Accès aux gestionnaire de SCENE
    scene::SceneManager* GetSceneManager();

    /// Accès aux gestionnaire de GUI
    gui::GuiManager* GetGuiManager();

    /// Accès aux gestionnaire du FPS
    ticks::FpsManager* GetFpsManager();

    /// Accès aux gestionnaire d'effets apres traitement
    ppe::PostProcessManager* GetPostProcessManager() const;

    /// Spécifier le taille de la fenetre
    void Setup(Vector2i viewport);

    /// Retourne la taille de la fenetre
    Vector2i GetWinSize() const;

    // Static ------------------------------------------------------------------

    /// Renvoi la version du moteur (Build Date)
    static std::string GetVersion();

    /// Renvoi la dossier courant
    static std::string GetCWD();

protected:
    scene::SceneManager* m_sceneManager;
    ppe::PostProcessManager* m_postProcessManager;
    ticks::FpsManager* m_fpsManager;
    gui::GuiManager* m_guiManager;
    EventManager* m_eventManager;

    Vector2i m_winSize;
};


}

#endif // TBE_H
