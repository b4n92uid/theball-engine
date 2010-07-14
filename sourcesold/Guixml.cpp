#include "gl_guixml.h"

using namespace std;
using namespace GLGUI;

#define LAYOUT 0
#define CONTROL 1

GuiObject* GuiXML::NewObject(TiXmlElement* elem, int& rettype)
{
	rettype= CONTROL;

	if(elem->ValueStr() == "layout")
	{
		int borderw=0, borderh=0;
		elem->Attribute("borderw",&borderw);
		elem->Attribute("borderh",&borderh);

		Layout::LayoutType type =
		strcmp(elem->Attribute("type"),"horizental") ? Layout::Vertical : Layout::Horizental;

		Layout* lay = new Layout(m_scrw,m_scrh,type);
		lay->setBorder(borderw,borderh);

			for(TiXmlElement* celem = elem->FirstChildElement(); celem; celem = celem->NextSiblingElement())
			{
				if(celem->ValueStr() == "strecthspace")
				lay->addStretchSpace();

				else
				{
					GuiObject* ctrl = NewObject(celem,rettype);

					if(ctrl)
					{
						if(rettype == CONTROL)
						{
							m_objects.push_back(ctrl);
							*lay += *ctrl;
						}

						else
						*lay += *(Layout*)(ctrl);
					}
				}
			}

		m_layouts.push_back(lay);

		rettype = LAYOUT;

		return lay;
	}

	else if(elem->ValueStr() == "gauge")
	{
		const char* label = elem->Attribute("label");
		const char* off = elem->Attribute("off");
		const char* on = elem->Attribute("on");
		const char* fontpath = elem->Attribute("fontpath");
		const char* id = elem->Attribute("id");

		int fontsize=0, width=0, height=0, x=0, y=0;

		elem->Attribute("fontsize",&fontsize);
		elem->Attribute("width",&width);
		elem->Attribute("height",&height);
		elem->Attribute("x",&x);
		elem->Attribute("y",&y);

		Gauge* ctrl = new Gauge(label);
		ctrl->setPos(x,y);
		ctrl->setSize(width,height);
		ctrl->setFont(fontpath,fontsize);
		ctrl->setBackground(off,on);

		if(id)
		ctrl->m_id = id;

		return ctrl;
	}

	else if(elem->ValueStr() == "image")
	{
		const char* path = elem->Attribute("path");
		const char* id = elem->Attribute("id");

		int width=0, height=0, x=0, y=0;

		elem->Attribute("width",&width);
		elem->Attribute("height",&height);
		elem->Attribute("x",&x);
		elem->Attribute("y",&y);

		Texture* ctrl = new Texture(path);
		ctrl->setPos(x,y);

		if(id)
		ctrl->m_id = id;

		return ctrl;
	}

	else if(elem->ValueStr() == "text")
	{
		const char* _label = elem->Attribute("label");
		const char* fontpath = elem->Attribute("fontpath");
		const char* id = elem->Attribute("id");

		int fontsize=0, x=0, y=0;

		elem->Attribute("fontsize",&fontsize);
		elem->Attribute("x",&x);
		elem->Attribute("y",&y);

		string label = _label;

		for(
			size_t pos=label.find("\\n") ;
			pos!=string::npos;
			pos=label.find("\\n")
		)
		label.replace(pos,2,"\n");

		TextFont* ctrl = new TextFont(fontpath,fontsize);
		ctrl->setPos(x,y);
		ctrl->Write(label.c_str());

		if(id)
		ctrl->m_id = id;

		return ctrl;
	}

	else if(elem->ValueStr() == "button")
	{
		const char* label = elem->Attribute("label");
		const char* minout = elem->Attribute("minout");
		const char* minover = elem->Attribute("minover");
		const char* fontpath = elem->Attribute("fontpath");
		const char* id = elem->Attribute("id");

		int fontsize=0, width=0, height=0, x=0, y=0;

		elem->Attribute("fontsize",&fontsize);
		elem->Attribute("width",&width);
		elem->Attribute("height",&height);
		elem->Attribute("x",&x);
		elem->Attribute("y",&y);

		Button* ctrl = new Button(label);
		ctrl->setPos(x,y);
		ctrl->setSize(width,height);
		ctrl->setFont(fontpath,fontsize);
		ctrl->setBackground(minout,minover);

		if(id)
		ctrl->m_id = id;

		return ctrl;
	}

	else
	throw GLExcep("GuiXML","Invalid Control (%s)",elem->Value());
}

void GuiXML::Render()
{
	for(size_t i=0 ; i<m_layouts.size() ; i++)
	m_layouts[i]->update();

	for(size_t i=0 ; i<m_objects.size() ; i++)
	m_objects[i]->Render();
}

void GuiXML::Parse(std::string filepath)
{
	TiXmlDocument doc;

	if(!doc.LoadFile(filepath))
	throw GLExcep("GuiXML","Load File Error (%s)",filepath.c_str());

	TiXmlElement* elem;

	elem = doc.FirstChildElement();

	if(!elem)
	throw GLExcep("GuiXML","Root Error");

	if(elem->ValueStr() != "gui")
	throw GLExcep("GuiXML","Invalid Root (%s)",elem->Value());

	for(
		elem = elem->FirstChildElement();
		elem;
		elem = elem->NextSiblingElement()
	)
	{
		int rettype = 0;
		GuiObject* ctrl = NewObject(elem,rettype);

		if(ctrl)
		m_objects.push_back(ctrl);
	}
}

 GuiXML::~GuiXML()
{

}

GuiXML::GuiXML(int scrw, int scrh)
{
	m_scrw = scrw;
	m_scrh = scrh;
}

 GuiXML::GuiXML(int scrw, int scrh, std::string filepath)
{
	m_scrw = scrw;
	m_scrh = scrh;

	Parse(filepath);
}

GuiObject* GuiXML::GetById(std::string id)
{
	for(size_t i=0 ; i<m_objects.size() ; i++)
	if(m_objects[i]->m_id == id)
	return m_objects[i];

	return NULL;
}
