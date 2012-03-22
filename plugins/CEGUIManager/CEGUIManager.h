/* 
 * File:   CEGUIManager.h
 * Author: b4n92uid
 *
 * Created on 22 mars 2012, 15:15
 */

#ifndef CEGUIMANAGER_H
#define	CEGUIMANAGER_H

#include <iostream>
#include <string>

#include "Mathematics.h"
#include "Texture.h"

#include <CEGUI.h>
#include <RendererModules/OpenGL/CEGUIOpenGLRenderer.h>

namespace tbe
{

class Device;
class EventManager;

namespace gui
{

class CEGUIManager
{
public:
    CEGUIManager();
    ~CEGUIManager();

    void setup(Vector2i scrsize);

    void render();

    void clearAll();

    void trasmitEvent(EventManager& e);

    void setScheme(std::string id);
    void setRessources(std::string path);

    void setViewport(Vector2i viewport);
    Vector2i getViewport() const;

    CEGUI::System* getSystem() const;
    CEGUI::Window* getRoot() const;
    CEGUI::WindowManager* getWindowManager() const;

protected:
    CEGUI::System* m_system;
    CEGUI::OpenGLRenderer* m_renderer;
    CEGUI::DefaultResourceProvider* m_defaultResProvider;
    CEGUI::WindowManager* m_winManager;
    CEGUI::Window* m_root;
    Vector2i m_viewport;
};

}
}

#endif	/* CEGUIMANAGER_H */
