#ifndef EVENTRECV_H
#define EVENTRECV_H

#include <string>
#include <map>

#include "Mathematics.h"

namespace tbe
{

/// \brief Structure de savegarde des evenements d'entrer par l'utilisateur

struct EventManager
{
public:

    /// Enumeration des type d'evenemtns

    enum NotifyType
    {
        EVENT_NO_EVENT,

        EVENT_MOUSE_MOVE,
        EVENT_MOUSE_DOWN,
        EVENT_MOUSE_UP,

        EVENT_KEY_DOWN,
        EVENT_KEY_UP,

        EVENT_WIN_QUIT,
        EVENT_WIN_RESIZE,
    };

    /// Enumeration de button de la souris

    enum MouseCode
    {
        MOUSE_BUTTON_LEFT,
        MOUSE_BUTTON_RIGHT,
        MOUSE_BUTTON_MIDDLE,
        MOUSE_BUTTON_WHEEL_UP,
        MOUSE_BUTTON_WHEEL_DOWN,
    };

    enum KeyCode
    {
        KEY_ESCAPE = 27,
        KEY_RETURN = 13,
        KEY_BACKSPACE = 8,
        KEY_SPACE = 32,
        KEY_TAB = 9,

        KEY_UP = 273,
        KEY_DOWN = 274,
        KEY_RIGHT = 275,
        KEY_LEFT = 276,
        KEY_INSERT = 277,
        KEY_DELETE = 127,
        KEY_HOME = 278,
        KEY_END = 279,
        KEY_PAGEUP = 280,
        KEY_PAGEDOWN = 281,

        KEY_F1 = 282,
        KEY_F2 = 283,
        KEY_F3 = 284,
        KEY_F4 = 285,
        KEY_F5 = 286,
        KEY_F6 = 287,
        KEY_F7 = 288,
        KEY_F8 = 289,
        KEY_F9 = 290,
        KEY_F10 = 291,
        KEY_F11 = 292,
        KEY_F12 = 293,
        KEY_F13 = 294,
        KEY_F14 = 295,
        KEY_F15 = 296,

        KEY_NUMLOCK = 300,
        KEY_CAPSLOCK = 301,
        KEY_SCROLLOCK = 302,
        KEY_RSHIFT = 303,
        KEY_LSHIFT = 304,
        KEY_RCTRL = 305,
        KEY_LCTRL = 306,
        KEY_RALT = 307,
        KEY_LALT = 308,
        KEY_RMETA = 309,
        KEY_LMETA = 310,
        KEY_LSUPER = 311, /* Left "Windows" key */
        KEY_RSUPER = 312, /* Right "Windows" key */
        KEY_MODE = 313, /* "Alt Gr" key */
    };

    EventManager()
    {
        notify = EVENT_NO_EVENT;
        lastActiveKey.first = 0;
        lastActiveMouse.first = 0;
    }

    NotifyType notify;

    long lastScanCode;
    
    std::pair<int, std::string> lastActiveMouse;
    std::pair<int, std::string> lastActiveKey;

    Vector2<int> mousePos, mousePosRel;

    std::map<int, bool> keyState;
    std::map<int, bool> mouseState;

    long lastPollTimestamp;
};

}

#endif // EVENTRECV_H
