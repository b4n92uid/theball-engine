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

void SDLDevice::window(std::string caption, Vector2i winsize, int bits, bool fullscreen, int multisamples)
{
    using namespace std;

    const SDL_version* sdlv = SDL_Linked_Version();

    cout << "SDL Init" << endl;
    cout << "Version: " << (int) sdlv->major << "." << (int) sdlv->minor << "." << (int) sdlv->minor << endl;
    cout << endl << endl;

    m_caption = caption;

    setVideoMode(winsize, bits, fullscreen, multisamples);

    init();
    setViewport(winsize);
}

void SDLDevice::setVideoMode(Vector2i winsize, int bits, bool fullscreen, int multisamples)
{
    // NOTE Requis pour réappliquer le multisampling
    if(SDL_WasInit(SDL_INIT_VIDEO))
    {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        SDL_InitSubSystem(SDL_INIT_VIDEO);
        SDL_EnableUNICODE(true);
    }

    m_viewport = winsize;

    const SDL_VideoInfo * videoInfo = SDL_GetVideoInfo();

    if(!m_viewport.x)
        m_viewport.x = videoInfo->current_w;

    if(!m_viewport.y)
        m_viewport.y = videoInfo->current_h;

    m_winBits = bits;
    m_winFullscreen = fullscreen;
    m_winMultiSamples = multisamples;

    Uint32 flags = SDL_OPENGL;

    if(m_winFullscreen)
        flags |= SDL_FULLSCREEN;

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, m_winMultiSamples);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
    SDL_WM_SetCaption(m_caption.c_str(), 0);

    if(SDL_SetVideoMode(m_viewport.x, m_viewport.y, m_winBits, flags) == NULL)
        throw Exception("SDLDevice::Window; Couldn't set specified video mode : %1%") % SDL_GetError();
}

void SDLDevice::pollEvent()
{
    bool eventOccur = true;

    m_eventManager->notify = EventManager::EVENT_NO_EVENT;

    SDL_Event sdlEvent = {0};
    SDL_PollEvent(&sdlEvent);

    if(sdlEvent.type == SDL_MOUSEMOTION)
    {
        m_eventManager->notify = EventManager::EVENT_MOUSE_MOVE;
        m_eventManager->mousePos = Vector2i(sdlEvent.motion.x, m_viewport.y - sdlEvent.motion.y);
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
                m_eventManager->lastActiveMouse.second = getMouseName(EventManager::MOUSE_BUTTON_LEFT);
                break;
            case SDL_BUTTON_RIGHT:
                m_eventManager->mouseState[EventManager::MOUSE_BUTTON_RIGHT] = true;
                m_eventManager->lastActiveMouse.first = EventManager::MOUSE_BUTTON_RIGHT;
                m_eventManager->lastActiveMouse.second = getMouseName(EventManager::MOUSE_BUTTON_RIGHT);
                break;
            case SDL_BUTTON_WHEELUP:
                m_eventManager->mouseState[EventManager::MOUSE_BUTTON_WHEEL_UP] = true;
                m_eventManager->lastActiveMouse.first = EventManager::MOUSE_BUTTON_WHEEL_UP;
                m_eventManager->lastActiveMouse.second = getMouseName(EventManager::MOUSE_BUTTON_WHEEL_UP);
                break;
            case SDL_BUTTON_WHEELDOWN:
                m_eventManager->mouseState[EventManager::MOUSE_BUTTON_WHEEL_DOWN] = true;
                m_eventManager->lastActiveMouse.first = EventManager::MOUSE_BUTTON_WHEEL_DOWN;
                m_eventManager->lastActiveMouse.second = getMouseName(EventManager::MOUSE_BUTTON_WHEEL_DOWN);
                break;
            case SDL_BUTTON_MIDDLE:
                m_eventManager->mouseState[EventManager::MOUSE_BUTTON_MIDDLE] = true;
                m_eventManager->lastActiveMouse.first = EventManager::MOUSE_BUTTON_MIDDLE;
                m_eventManager->lastActiveMouse.second = getMouseName(EventManager::MOUSE_BUTTON_MIDDLE);
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
                m_eventManager->lastActiveMouse.second = getMouseName(EventManager::MOUSE_BUTTON_LEFT);
                break;
            case SDL_BUTTON_RIGHT:
                m_eventManager->mouseState[EventManager::MOUSE_BUTTON_RIGHT] = false;
                m_eventManager->lastActiveMouse.first = EventManager::MOUSE_BUTTON_RIGHT;
                m_eventManager->lastActiveMouse.second = getMouseName(EventManager::MOUSE_BUTTON_RIGHT);
                break;
            case SDL_BUTTON_WHEELUP:
                m_eventManager->mouseState[EventManager::MOUSE_BUTTON_WHEEL_UP] = false;
                m_eventManager->lastActiveMouse.first = EventManager::MOUSE_BUTTON_WHEEL_UP;
                m_eventManager->lastActiveMouse.second = getMouseName(EventManager::MOUSE_BUTTON_WHEEL_UP);
                break;
            case SDL_BUTTON_WHEELDOWN:
                m_eventManager->mouseState[EventManager::MOUSE_BUTTON_WHEEL_DOWN] = false;
                m_eventManager->lastActiveMouse.first = EventManager::MOUSE_BUTTON_WHEEL_DOWN;
                m_eventManager->lastActiveMouse.second = getMouseName(EventManager::MOUSE_BUTTON_WHEEL_DOWN);
                break;
            case SDL_BUTTON_MIDDLE:
                m_eventManager->mouseState[EventManager::MOUSE_BUTTON_MIDDLE] = false;
                m_eventManager->lastActiveMouse.first = EventManager::MOUSE_BUTTON_MIDDLE;
                m_eventManager->lastActiveMouse.second = getMouseName(EventManager::MOUSE_BUTTON_MIDDLE);
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
            sdlEvent.key.keysym.sym = (SDLKey) sdlEvent.key.keysym.unicode;

        m_eventManager->keyState[(EventManager::KeyCode)sdlEvent.key.keysym.sym] = true;

        m_eventManager->lastScanCode = sdlEvent.key.keysym.scancode;

        m_eventManager->lastActiveKey.first = sdlEvent.key.keysym.sym;
        m_eventManager->lastActiveKey.second = getKeyName(sdlEvent.key.keysym.sym);
    }

    else if(sdlEvent.type == SDL_KEYUP)
    {
        m_eventManager->notify = EventManager::EVENT_KEY_UP;

        if(!m_eventManager->keyState[(EventManager::KeyCode)sdlEvent.key.keysym.sym])
            m_eventManager->keyState[(EventManager::KeyCode)m_keyStateTmp[sdlEvent.key.keysym.sym]] = false;
        else
            m_eventManager->keyState[(EventManager::KeyCode)sdlEvent.key.keysym.sym] = false;

        m_eventManager->lastScanCode = sdlEvent.key.keysym.scancode;

        m_eventManager->lastActiveKey.first = sdlEvent.key.keysym.sym;
        m_eventManager->lastActiveKey.second = getKeyName(sdlEvent.key.keysym.sym);
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

    m_eventManager->lastPollTimestamp = m_timestamp.getEsplanedTime();
}

int SDLDevice::getWinBits() const
{
    return m_winBits;
}

bool SDLDevice::isWinFullscreen() const
{
    return m_winFullscreen;
}

int SDLDevice::getWinMultiSamples() const
{
    return m_winMultiSamples;
}

void SDLDevice::setGrabInput(bool stat)
{
    m_mouseGrab = stat;

    SDL_WM_GrabInput(stat ? SDL_GRAB_ON : SDL_GRAB_OFF);
}

void SDLDevice::setMouseVisible(bool stat)
{
    m_mouseVisible = stat;

    SDL_ShowCursor(stat);
}

std::string SDLDevice::getMouseName(int mouseCode)
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

std::string SDLDevice::getKeyName(int keyCode)
{
    std::string keyName = SDL_GetKeyName((SDLKey) keyCode);
    keyName[0] = toupper(keyName[0]);

    return keyName;
}

std::vector<Vector2i> SDLDevice::getAvilableSceeenSize()
{
    std::vector<Vector2i> vrects;

    SDL_Rect** rects = SDL_ListModes(NULL, SDL_FULLSCREEN | SDL_HWSURFACE | SDL_OPENGL);

    if(!rects)
        throw Exception("Device::GetAvilableSceeenSize; Warning no screen available");

    else if(-1 == (int) rects)
        throw Exception("Device::GetAvilableSceeenSize; Resolution is restricted");

    else

        for(unsigned i = 0; rects[i]; i++)
            vrects.push_back(Vector2i(rects[i]->w, rects[i]->h));

    std::reverse(vrects.begin(), vrects.end());

    return vrects;
}

void SDLDevice::beginScene()
{
    Device::beginScene();
}

void SDLDevice::endScene()
{
    Device::endScene();

    SDL_GL_SwapBuffers();
}
