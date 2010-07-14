#ifndef CONSOLE_H
#define CONSOLE_H

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdarg>
#include <gl/gl.h>
#include <gl/glu.h>

#include "gl_textfont.h"

class Console : public std::stringstream
{
	public:

	Console();
	Console(int screenw, int screenh);
	~Console();

	void Console::SetFont(std::string fontpath, int fontsize);
	void Console::SetScreen(int screenw, int screenh);
	void Render();

	protected:

	TextFont m_text;
	int m_screenw, m_screenh;
};

#endif
