#ifndef GUIXML_H
#define GUIXML_H

#include <tinyXML.h>
#include "gl_gui.h"

namespace GLGUI
{

class GuiXML
{
	public:

		GuiXML(int scrw, int scrh);
		GuiXML(int scrw, int scrh, std::string filepath);

		~GuiXML();

		void GuiXML::Parse(std::string filepath);
		void GuiXML::Render();

		GuiObject* GuiXML::GetById(std::string id);

		void injectMousePos(int mouse_x, int mouse_y)
		{for(size_t i=0 ; i<m_objects.size() ; i++)
		m_objects[i]->injectMousePos(mouse_x,mouse_y);}

		void injectKey(unsigned char key)
		{for(size_t i=0 ; i<m_objects.size() ; i++)
		m_objects[i]->injectKey(key);}

		void injectMouseClick(bool click)
		{for(size_t i=0 ; i<m_objects.size() ; i++)
		m_objects[i]->injectMouseClick(click);}

	protected:

		GuiObject* GuiXML::NewObject(TiXmlElement* elem, int& rettype);

		std::string m_filepath;
		std::vector<GuiObject*> m_objects;
		std::vector<Layout*> m_layouts;

		int m_scrw, m_scrh;

	private:
};

}

#endif // GUIXML_H
