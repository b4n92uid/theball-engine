#ifndef OGLEESDL_H
#define OGLEESDL_H

#include "Tbe.h"
#include <sdl.h>

#ifdef __DEBUG__
#undef main
#endif

class TbeSdl : public Tbe
{
	private:

		TbeSdl();
		virtual ~TbeSdl();

	public:

		// Fenetre
		void Window(std::string caption, int w, int h, int bits, bool fullscreen);

		// Rendue
		void EndScene();
		void BeginScene();

		// Boucle
		void GetEvent();

		// Cursor
		void SetGrabInput(bool stat);
		void SetMouseVisible(bool stat);

	protected:

};

#endif // OGLEESDL_H
