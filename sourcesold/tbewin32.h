#ifndef OGLEEWIN32_H
#define OGLEEWIN32_H

#include "Tbe.h"
#include <windows.h>

class TbeWin32 : public Tbe
{
	private:

		TbeWin32();
		virtual ~TbeWin32();

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

		// Windows
		HDC m_dc;
		HGLRC m_rc;
		HWND m_hwnd;

};

#endif // OGLEEWIN32_H
