#include "tbewin32.h"

static Tbe* ge = NULL;
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

TbeWin32::TbeWin32()
{
	m_dc = NULL;
	m_rc = NULL;

	ge = this;
}

TbeWin32::~TbeWin32()
{
	//dtor
}

void TbeWin32::Window(std::string caption, int w, int h, int bits, bool fullscreen)
{
	Tbe::Window(caption,w,h,bits,fullscreen);

	if(m_winfullscreen)
	{
		DEVMODE dm;

		// use default values from current setting
		memset(&dm, 0, sizeof(dm));
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);

		dm.dmSize = sizeof(dm);
		dm.dmPelsWidth = m_winsize.x;
		dm.dmPelsHeight = m_winsize.y;
		dm.dmBitsPerPel = m_winbits;
		dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

		LONG res = ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
		if (res != DISP_CHANGE_SUCCESSFUL)
		{
			// try again without forcing display frequency
			dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
			res = ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
		}

		switch(res)
		{
			case DISP_CHANGE_SUCCESSFUL:
				break;
			case DISP_CHANGE_RESTART:
				throw GLExcep("Switch to fullscreen: The computer must be restarted in order for the graphics mode to work.");
				break;
			case DISP_CHANGE_BADFLAGS:
				throw GLExcep("Switch to fullscreen: An invalid set of flags was passed in.");
				break;
			case DISP_CHANGE_BADPARAM:
				throw GLExcep("Switch to fullscreen: An invalid parameter was passed in. This can include an invalid flag or combination of flags.");
				break;
			case DISP_CHANGE_FAILED:
				throw GLExcep("Switch to fullscreen: The display driver failed the specified graphics mode.");
				break;
			case DISP_CHANGE_BADMODE:
				throw GLExcep("Switch to fullscreen: The graphics mode is not supported.");
				break;
			default:
				throw GLExcep("An unknown error occured while changing to fullscreen.");
				break;
		}
	}

	const char* szClassName = "01231992";

	/// Creation class

    WNDCLASSEX wincl;
    wincl.hInstance = GetModuleHandle(0);
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    if (!RegisterClassEx (&wincl))
        return;

	/// Creation fenetre

	if(m_winfullscreen)
    m_hwnd = CreateWindowEx (0,szClassName,caption.c_str(),WS_POPUP|WS_VISIBLE,
							CW_USEDEFAULT,CW_USEDEFAULT,m_winsize.x,m_winsize.y,HWND_DESKTOP,NULL,GetModuleHandle(0),this);
	else
	m_hwnd = CreateWindowEx (0,szClassName,caption.c_str(),WS_OVERLAPPEDWINDOW|WS_VISIBLE,
							CW_USEDEFAULT,CW_USEDEFAULT,m_winsize.x,m_winsize.y,HWND_DESKTOP,NULL,GetModuleHandle(0),this);

	{
		RECT rect;
		GetClientRect(m_hwnd,&rect);
		m_winsize.x = rect.right-rect.left;
		m_winsize.y = rect.bottom-rect.top;
	}

	/// Initialisation Fenetre OpenGL

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR), 	//taille du descripteur de format
		1, 	//version
		PFD_SUPPORT_OPENGL |
		PFD_DRAW_TO_WINDOW |
		PFD_DOUBLEBUFFER, 	//Propriété
		PFD_TYPE_RGBA, 	//Mode de couleurs
		m_winbits, 	//Bits de couleur
		0, 0, 0, 0, 0, 0, 	//Paramètres des couleurs
		0,0, 	//Paramètres alpha
		0,0, 0, 0, 0, 	//Paramètres du buffer d'accumulation
		32, 	//Bits de profondeur
		0, 	//Bits du buffer stencil
		0, 	//Nombre de buffers auxiliaires
		0, 	//ignoré (obsolète)
		0, 	//réservé/code>
		0, 	//ignoré (obsolète)
		0, 	//Couleur de transparence
		0 	//Ignoré (obsolète)
	};

	m_dc = GetDC(m_hwnd);

	int pixelFormat = ChoosePixelFormat(m_dc, &pfd);
	SetPixelFormat(m_dc, pixelFormat, &pfd);

	HGLRC m_rc = wglCreateContext(m_dc);
	wglMakeCurrent(m_dc, m_rc);

	m_scenemng.Init(m_winsize.x,m_winsize.y,60);
	m_guimng.Init(m_winsize.x,m_winsize.y);
}

void TbeWin32::EndScene()
{
	Tbe::EndScene();

	SwapBuffers(m_dc);
}

void TbeWin32::BeginScene()
{
	Tbe::BeginScene();
}

void TbeWin32::GetEvent()
{
	MSG m_messages = {0};

	if(PeekMessage (&m_messages, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&m_messages);
		DispatchMessage(&m_messages);
	}

	Tbe::GetEvent();
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_CREATE)
	{

	}

	else if(message == WM_SIZE)
	{
		ge->Resize(LOWORD(lParam),HIWORD(lParam));
	}

	else if(message == WM_MOUSEMOVE)
	{
		ge->InjectMousePos(LOWORD(lParam),ge->GetSize().y-HIWORD(lParam));
		ge->InjectNotify(EventRecv::MOUSE_MOVE);
	}

	else if(message == WM_LBUTTONDOWN)
	{
		ge->InjectMouseClick(true);
		ge->InjectNotify(EventRecv::MOUSE_DOWN);
	}

	else if(message == WM_LBUTTONUP)
	{
		ge->InjectMouseClick(false);
		ge->InjectNotify(EventRecv::MOUSE_UP);
	}

	else if(message == WM_KEYDOWN)
	{
		BYTE KeyState[256] = {0};
		GetKeyboardState(KeyState);
		WORD AsciiKey = 0;
		ToAscii(wParam, 0 ,KeyState,&AsciiKey,0);

		ge->InjectKey(AsciiKey,true);
		ge->InjectNotify(EventRecv::KEY_DOWN);
	}

	else if(message == WM_KEYUP)
	{
		BYTE KeyState[256] = {0};
		GetKeyboardState(KeyState);
		WORD AsciiKey = 0;
		ToAscii(wParam, 0 ,KeyState,&AsciiKey,0);

		ge->InjectKey(AsciiKey,false);
		ge->InjectNotify(EventRecv::KEY_UP);
	}

	else if(message == WM_DESTROY || message == WM_QUIT || message == WM_CLOSE)
	{
		ge->InjectNotify(EventRecv::QUIT);
	}

	else
	return DefWindowProc (hwnd, message, wParam, lParam);

    return 0;
}

void TbeWin32::SetGrabInput(bool stat)
{
	Tbe::SetGrabInput(stat);
}

void TbeWin32::SetMouseVisible(bool stat)
{
	m_mouse_visible = stat;
	ShowCursor(stat);
}
