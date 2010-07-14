#include "gl_console.h"

using namespace std;

void Console::SetScreen(int screenw, int screenh)
{
	m_screenw = screenw;
	m_screenh = screenh;
}

void Console::SetFont(std::string fontpath, int fontsize)
{
	m_text.setFont(fontpath,fontsize);
}

void Console::Render()
{
	FTBBox box = m_text.getFont().BBox("X");
	string buf = str();

	int x=0, y=m_screenh/3*2, w=m_screenw, h=m_screenh/3;
	int lineshown = 8;//int(h / (box.Upper().Y() - box.Lower().Y()));
	int linecount = 0;

	for(int i=buf.length()-1 ; i ; i--)
	{
		if(buf[i] == '\n')
		{
			linecount++;

			if(linecount == lineshown)
			{
				str(&buf[i+1]);
				break;
			}
		}
	}

	// rendue

	glPushAttrib(GL_CURRENT_BIT|GL_ENABLE_BIT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glColor4f(0,0,0,0.9);
		glBegin(GL_TRIANGLE_STRIP);
		glVertex2d(x,y+h);
		glVertex2d(x,y);
		glVertex2d(x+w,y+h);
		glVertex2d(x+w,y);
		glEnd();

		m_text.Print(5,y+5,str().c_str());

    glPopAttrib();
}

 Console::~Console()
{

}

 Console::Console()
{

}

 Console::Console(int screenw, int screenh)
{
	m_screenw = screenw;
	m_screenh = screenh;
}

