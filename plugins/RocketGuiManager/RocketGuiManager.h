/* 
 * File:   RocketGuiManager.h
 * Author: b4n92uid
 *
 * Created on 23 mars 2012, 15:09
 */

#ifndef ROCKETGUIMANAGER_H
#define	ROCKETGUIMANAGER_H

#include <Tbe.h>
#include <Rocket/Core.h>
#include <Rocket/Controls.h>

namespace tbe
{

class Device;
class EventManager;

namespace gui
{

class ShellRenderInterfaceOpenGL;
class ShellSystemInterface;
class ShellFileInterface;

class RocketGuiManager
{
public:
    RocketGuiManager();
    ~RocketGuiManager();

    /// Initialisation 2D
    void setup(Vector2i viewport);

    /// Efface tout les controles
    void clearAll();

    /// Rendue
    void render();

    /// Transmition des evenement aux controle
    void trasmitEvent(EventManager& e);

    void setViewport(Vector2i viewport);
    Vector2i getViewport() const;

    void loadFonts(std::string dirpath);
    
    void addPath(std::string dirpath);

    Rocket::Core::Context* getContext() const;

private:
    Vector2i m_viewport;
    Rocket::Core::Context* m_context;

    ShellRenderInterfaceOpenGL* m_renderInterface;
    ShellSystemInterface* m_systemInterface;
    ShellFileInterface* m_fileInterface;
};

}
}

#endif	/* ROCKETGUIMANAGER_H */
