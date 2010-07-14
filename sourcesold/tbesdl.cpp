#include "tbesdl.h"

TbeSdl::TbeSdl()
{
	SDL_Init(SDL_INIT_VIDEO);
}

TbeSdl::~TbeSdl()
{
	SDL_Quit();
}


void TbeSdl::Window(std::string caption, int w, int h, int bits, bool fullscreen)
{
	Tbe::Window(caption,w,h,bits,fullscreen);

	Uint32 flags = SDL_OPENGL|SDL_RESIZABLE;

	if(m_winfullscreen)
	flags |= SDL_FULLSCREEN;

	/// Creation fenetre
	SDL_WM_SetCaption(caption.c_str(),0);
	SDL_SetVideoMode(m_winsize.x,m_winsize.y,bits,flags);

	m_scenemng.Init(m_winsize.x,m_winsize.y,60);
	m_guimng.Init(m_winsize.x,m_winsize.y);
}

void TbeSdl::EndScene()
{
	Tbe::EndScene();
	SDL_GL_SwapBuffers();
}

void TbeSdl::BeginScene()
{
	Tbe::BeginScene();
}

void TbeSdl::GetEvent()
{
	InjectNotify(EventRecv::NO_EVENT);

	SDL_Event event = {0};
	SDL_PollEvent(&event);

	if(event.type == SDL_VIDEORESIZE)
	{
		Resize(event.resize.w,event.resize.h);
	}

	else if(event.type == SDL_MOUSEMOTION)
	{
		InjectMousePos(event.motion.x,GetSize().y-event.motion.y);
		m_mouseposrel.x = event.motion.xrel;
		m_mouseposrel.y = event.motion.yrel;

		InjectNotify(EventRecv::MOUSE_MOVE);
	}

	else if(event.type == SDL_MOUSEBUTTONDOWN)
	{
		InjectMouseClick(true);
		InjectNotify(EventRecv::MOUSE_DOWN);
	}

	else if(event.type == SDL_MOUSEBUTTONUP)
	{
		InjectMouseClick(false);
		InjectNotify(EventRecv::MOUSE_UP);
	}

	else if(event.type == SDL_KEYDOWN)
	{
		InjectKey(event.key.keysym.sym,true);
		InjectNotify(EventRecv::KEY_DOWN);
	}

	else if(event.type == SDL_KEYUP)
	{
		InjectKey(event.key.keysym.sym,false);
		InjectNotify(EventRecv::KEY_UP);
	}

	else if(event.type == SDL_QUIT)
	{
		InjectNotify(EventRecv::QUIT);
	}

	Tbe::GetEvent();
}

void TbeSdl::SetGrabInput(bool stat)
{
	Tbe::SetGrabInput(stat);

	SDL_WM_GrabInput(stat?SDL_GRAB_ON:SDL_GRAB_OFF);
}

void TbeSdl::SetMouseVisible(bool stat)
{
	m_mouse_visible = stat;
	SDL_ShowCursor(stat);
}
