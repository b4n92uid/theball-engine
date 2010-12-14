/*
 * File:   SDLDevice.cpp
 * Author: b4n92uid
 *
 * Created on 15 mai 2010, 22:24
 */

#include "SDLDevice.h"

using namespace tbe;

SDLDevice::SDLDevice()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_EnableUNICODE(true);

    m_mouseGrab = false;
    m_mouseVisible = true;
    m_winBits = 0;
    m_winFullscreen = false;
}

SDLDevice::~SDLDevice()
{
    SDL_Quit();
}

void SDLDevice::Window(std::string caption, Vector2i winsize, int bits, bool fullscreen, int multisamples)
{
    using namespace std;

    // NOTE Requis pour resp�cifier le multisampling
    if(SDL_WasInit(SDL_INIT_VIDEO))
    {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        SDL_InitSubSystem(SDL_INIT_VIDEO);
        SDL_EnableUNICODE(true);
    }
    // --

    m_caption = caption;
    m_viewportSize = winsize;
    m_winBits = bits;
    m_winFullscreen = fullscreen;
    m_winMultiSamples = multisamples;

    Uint32 flags = SDL_OPENGL;

    if(m_winFullscreen)
        flags |= SDL_FULLSCREEN;

    const SDL_VideoInfo * videoInfo = SDL_GetVideoInfo();

    if(!m_viewportSize.x)
        m_viewportSize.x = videoInfo->current_w;

    if(!m_viewportSize.y)
        m_viewportSize.y = videoInfo->current_h;

    int returnState = 0;

    returnState = SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, m_winMultiSamples);
    if(returnState == -1)
        cout << "SDLDevice::Window; Multisamples setting failed " << m_winMultiSamples << endl;

    returnState = SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    if(returnState == -1)
        cout << "SDLDevice::Window; Double Buffer setting failed 1" << endl;

    SDL_WM_SetCaption(caption.c_str(), 0);

    if(SDL_SetVideoMode(m_viewportSize.x, m_viewportSize.y, bits, flags) == NULL)
        throw Exception("SDLDevice::Window; Couldn't set specified video mode : %s", SDL_GetError());

    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &returnState);
    if(returnState != m_winMultiSamples)
        cout << "SDLDevice::Window; Multisamples incorrect value " << returnState << endl;

    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &returnState);
    if(returnState != 1)
        cout << "SDLDevice::Window; Double Buffer incorrect value " << returnState << endl;

    char* error = SDL_GetError();

    if(strlen(error))
        cout << "SDLDevice::Window; SDL error: " << error << endl;

    Init();

    SetViewportSize(m_viewportSize);
}

void SDLDevice::PollEvent()
{
    bool eventOccur = true;

    m_eventManager->notify = EventManager::EVENT_NO_EVENT;

    SDL_Event sdlEvent = {0};
    SDL_PollEvent(&sdlEvent);

    if(sdlEvent.type == SDL_MOUSEMOTION)
    {
        m_eventManager->notify = EventManager::EVENT_MOUSE_MOVE;
        m_eventManager->mousePos = Vector2i(sdlEvent.motion.x, m_viewportSize.y - sdlEvent.motion.y);
        m_eventManager->mousePosRel = Vector2i(sdlEvent.motion.xrel, -sdlEvent.motion.yrel);
    }

    else if(sdlEvent.type == SDL_MOUSEBUTTONDOWN)
    {
        m_eventManager->notify = EventManager::EVENT_MOUSE_DOWN;

        switch(sdlEvent.button.button)
        {
            case SDL_BUTTON_LEFT:
                m_eventManager->mouseState[EventManager::MOUSE_BUTTON_LEFT] = true;
                m_eventManager->lastActiveMouse.first = EventManager::MOUSE_BUTTON_LEFT;
                m_eventManager->lastActiveMouse.second = GetMouseName(EventManager::MOUSE_BUTTON_LEFT);
                break;
            case SDL_BUTTON_RIGHT:
                m_eventManager->mouseState[EventManager::MOUSE_BUTTON_RIGHT] = true;
                m_eventManager->lastActiveMouse.first = EventManager::MOUSE_BUTTON_RIGHT;
                m_eventManager->lastActiveMouse.second = GetMouseName(EventManager::MOUSE_BUTTON_RIGHT);
                break;
            case SDL_BUTTON_WHEELUP:
                m_eventManager->mouseState[EventManager::MOUSE_BUTTON_WHEEL_UP] = true;
                m_eventManager->lastActiveMouse.first = EventManager::MOUSE_BUTTON_WHEEL_UP;
                m_eventManager->lastActiveMouse.second = GetMouseName(EventManager::MOUSE_BUTTON_WHEEL_UP);
                break;
            case SDL_BUTTON_WHEELDOWN:
                m_eventManager->mouseState[EventManager::MOUSE_BUTTON_WHEEL_DOWN] = true;
                m_eventManager->lastActiveMouse.first = EventManager::MOUSE_BUTTON_WHEEL_DOWN;
                m_eventManager->lastActiveMouse.second = GetMouseName(EventManager::MOUSE_BUTTON_WHEEL_DOWN);
                break;
            case SDL_BUTTON_MIDDLE:
                m_eventManager->mouseState[EventManager::MOUSE_BUTTON_MIDDLE] = true;
                m_eventManager->lastActiveMouse.first = EventManager::MOUSE_BUTTON_MIDDLE;
                m_eventManager->lastActiveMouse.second = GetMouseName(EventManager::MOUSE_BUTTON_MIDDLE);
                break;
        }
    }

    else if(sdlEvent.type == SDL_MOUSEBUTTONUP)
    {
        m_eventManager->notify = EventManager::EVENT_MOUSE_UP;

        switch(sdlEvent.button.button)
        {
            case SDL_BUTTON_LEFT:
                m_eventManager->mouseState[EventManager::MOUSE_BUTTON_LEFT] = false;
                m_eventManager->lastActiveMouse.first = EventManager::MOUSE_BUTTON_LEFT;
                m_eventManager->lastActiveMouse.second = GetMouseName(EventManager::MOUSE_BUTTON_LEFT);
                break;
            case SDL_BUTTON_RIGHT:
                m_eventManager->mouseState[EventManager::MOUSE_BUTTON_RIGHT] = false;
                m_eventManager->lastActiveMouse.first = EventManager::MOUSE_BUTTON_RIGHT;
                m_eventManager->lastActiveMouse.second = GetMouseName(EventManager::MOUSE_BUTTON_RIGHT);
                break;
            case SDL_BUTTON_WHEELUP:
                m_eventManager->mouseState[EventManager::MOUSE_BUTTON_WHEEL_UP] = false;
                m_eventManager->lastActiveMouse.first = EventManager::MOUSE_BUTTON_WHEEL_UP;
                m_eventManager->lastActiveMouse.second = GetMouseName(EventManager::MOUSE_BUTTON_WHEEL_UP);
                break;
            case SDL_BUTTON_WHEELDOWN:
                m_eventManager->mouseState[EventManager::MOUSE_BUTTON_WHEEL_DOWN] = false;
                m_eventManager->lastActiveMouse.first = EventManager::MOUSE_BUTTON_WHEEL_DOWN;
                m_eventManager->lastActiveMouse.second = GetMouseName(EventManager::MOUSE_BUTTON_WHEEL_DOWN);
                break;
            case SDL_BUTTON_MIDDLE:
                m_eventManager->mouseState[EventManager::MOUSE_BUTTON_MIDDLE] = false;
                m_eventManager->lastActiveMouse.first = EventManager::MOUSE_BUTTON_MIDDLE;
                m_eventManager->lastActiveMouse.second = GetMouseName(EventManager::MOUSE_BUTTON_MIDDLE);
                break;
        }
    }

    else if(sdlEvent.type == SDL_KEYDOWN)
    {
        m_eventManager->notify = EventManager::EVENT_KEY_DOWN;

        //        if(sdlEvent.key.keysym.mod & KMOD_LCTRL)
        //            sdlEvent.key.keysym.unicode = EventManager::KEY_LCTRL;
        //
        //        if(sdlEvent.key.keysym.mod & KMOD_RCTRL)
        //            sdlEvent.key.keysym.unicode = EventManager::KEY_RCTRL;

        m_keyStateTmp[sdlEvent.key.keysym.sym] = sdlEvent.key.keysym.unicode;

        if(sdlEvent.key.keysym.unicode && sdlEvent.key.keysym.unicode < 256)
            sdlEvent.key.keysym.sym = (SDLKey)sdlEvent.key.keysym.unicode;

        m_eventManager->keyState[(EventManager::KeyCode)sdlEvent.key.keysym.sym] = true;

        m_eventManager->lastActiveKey.first = sdlEvent.key.keysym.sym;
        m_eventManager->lastActiveKey.second = GetKeyName(sdlEvent.key.keysym.sym);
    }

    else if(sdlEvent.type == SDL_KEYUP)
    {
        m_eventManager->notify = EventManager::EVENT_KEY_UP;

        if(!m_eventManager->keyState[(EventManager::KeyCode)sdlEvent.key.keysym.sym])
            m_eventManager->keyState[(EventManager::KeyCode)m_keyStateTmp[sdlEvent.key.keysym.sym]] = false;
        else
            m_eventManager->keyState[(EventManager::KeyCode)sdlEvent.key.keysym.sym] = false;

        m_eventManager->lastActiveKey.first = sdlEvent.key.keysym.sym;
        m_eventManager->lastActiveKey.second = GetKeyName(sdlEvent.key.keysym.sym);
    }

    else if(sdlEvent.type == SDL_VIDEORESIZE)
    {
        m_eventManager->notify = EventManager::EVENT_WIN_RESIZE;
    }

    else if(sdlEvent.type == SDL_QUIT)
    {
        m_eventManager->notify = EventManager::EVENT_WIN_QUIT;
    }

    else
        eventOccur = false;

    if(eventOccur)
        m_guiManager->TrasmitEvent(*m_eventManager);
}

int SDLDevice::GetWinBits() const
{
    return m_winBits;
}

bool SDLDevice::IsWinFullscreen() const
{
    return m_winFullscreen;
}

int SDLDevice::GetWinMultiSamples() const
{
    return m_winMultiSamples;
}

void SDLDevice::SetGrabInput(bool stat)
{
    m_mouseGrab = stat;

    SDL_WM_GrabInput(stat ? SDL_GRAB_ON : SDL_GRAB_OFF);
}

void SDLDevice::SetMouseVisible(bool stat)
{
    m_mouseVisible = stat;

    SDL_ShowCursor(stat);
}

std::string SDLDevice::GetMouseName(int mouseCode)
{
    switch(mouseCode)
    {
        case EventManager::MOUSE_BUTTON_LEFT: return "Mouse left";
        case EventManager::MOUSE_BUTTON_RIGHT: return "Mouse right";
        case EventManager::MOUSE_BUTTON_MIDDLE: return "Mouse middle";
        case EventManager::MOUSE_BUTTON_WHEEL_UP: return "Mouse wheelup";
        case EventManager::MOUSE_BUTTON_WHEEL_DOWN: return "Mouse wheeldown";
    }

    std::stringstream exp;
    exp << mouseCode;
    return exp.str();
}

std::string SDLDevice::GetKeyName(int keyCode)
{
    std::string keyName = SDL_GetKeyName((SDLKey)keyCode);
    keyName[0] = toupper(keyName[0]);

    return keyName;
}

std::vector<Vector2i> SDLDevice::GetAvilableSceeenSize()
{
    std::vector<Vector2i> vrects;

    SDL_Rect** rects = SDL_ListModes(NULL, SDL_FULLSCREEN | SDL_HWSURFACE | SDL_OPENGL);

    if(!rects)
        throw Exception("Device::GetAvilableSceeenSize; Warning no screen available");

    else if(-1 == (int)rects)
        throw Exception("Device::GetAvilableSceeenSize; Resolution is restricted");

    else

        for(unsigned i = 0; rects[i]; i++)
            vrects.push_back(Vector2i(rects[i]->w, rects[i]->h));

    std::reverse(vrects.begin(), vrects.end());

    return vrects;
}

void SDLDevice::BeginScene()
{
    Device::BeginScene();
}

void SDLDevice::EndScene()
{
    Device::EndScene();

    SDL_GL_SwapBuffers();
}
