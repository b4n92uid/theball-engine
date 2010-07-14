#include <SDL.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include "gl_camera.h"
#include "gl_tools.h"

#undef main

#define SCRW 800
#define SCRH 640

#define TEXSIZEW 512
#define TEXSIZEH 512

void RenderScene()
{
	static float rotat = 0;

	glBindTexture (GL_TEXTURE_2D, 0);

	glPushMatrix();
	glRotatef(rotat,0,1,0);
	rotat++;
	glTranslatef(10,10,0);
	glColor3f(1,1,0);
	glDrawCube(1);
	glTranslatef(-20,0,0);
	glColor3f(0,0,1);
	glDrawCube(2);
	glTranslatef(10,0,10);
	glColor3f(1,0,0);
	glDrawCube(3);
	glTranslatef(0,0,-20);
	glColor3f(0,1,0);
	glDrawCube(4);
	glPopMatrix();
}

void mainapp()
{
	/// SDL
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetCaption("Rendu dans texture Test",0);
	SDL_SetVideoMode(SCRW,SCRH,32,SDL_OPENGL);
	SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_ShowCursor(false);

	bool exit = false;
	SDL_Event event;

	/// OpenGL

	glMatrixMode(GL_PROJECTION);
	gluPerspective(70,(float)SCRW/SCRH,1,2000);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glClearColor(1,1,1,1);

	Camera cam;

	float lightpos[4] = {1,1,1,0};

	/// Texture

	GLuint m_texture;

	// Allocation de la memoir largeur * hauteur * nombre de composant RGB (3)
	unsigned int *pTexture = new unsigned int [TEXSIZEW * TEXSIZEH * 3];
	memset (pTexture, 0, TEXSIZEW * TEXSIZEH * 3 * sizeof(unsigned int));

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXSIZEW, TEXSIZEH, 0, GL_RGB, GL_UNSIGNED_INT, pTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	delete [] pTexture;

	while(!exit)
	{
		event.type = SDL_NOEVENT;
		SDL_PollEvent(&event);

		if(event.type == SDL_MOUSEMOTION)
		cam.rotate(event.motion.xrel,event.motion.yrel);

		if(event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
		exit = true;

		/// Configurer ecran pour taille texture

		glViewport (0, 0, TEXSIZEW, TEXSIZEH);
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity ();
		gluPerspective (90.0f, (GLfloat)TEXSIZEW / (GLfloat)TEXSIZEH, .4f, 1000);
		glMatrixMode (GL_MODELVIEW);
		glLoadIdentity ();

		/// Rendue dans la texture

		// Effacement
		glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		//gluLookAt(0,1,0,0,20,0,0,0,-1);

		cam.m_pos = 0;
		cam.m_pos -= cam.m_target.Normalize()*50;
		cam.engine();

		// Dessin la scene
		RenderScene();

		// Copie la scène dans la texture identifié par g_uiTexture
		glBindTexture (GL_TEXTURE_2D, m_texture);
		glCopyTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, 0, 0, TEXSIZEW, TEXSIZEH, 0);

		/// Configurer ecran pour taille normal

		glViewport (0, 0, SCRW, SCRH);
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity ();
		gluPerspective (45.0f, (GLfloat)SCRW / (GLfloat)SCRH, .4f, 1000);
		glMatrixMode (GL_MODELVIEW);
		glLoadIdentity ();

		/// Rendue a l'ecran

		// Effacement
		glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		cam.m_pos = 0;
		cam.m_pos -= cam.m_target.Normalize()*50;
		cam.engine();

		// Dessin la scene avec la texture de la 1ere pass
		glBindTexture (GL_TEXTURE_2D, m_texture);

		glColor3f(0.5,0.5,0.5);
		glDrawCube(3);

		RenderScene();

		/// *

		glLightfv(GL_LIGHT0,GL_POSITION,lightpos);

		glFlush();
		SDL_GL_SwapBuffers();
	}

	glDeleteTextures(1,&m_texture);

	SDL_Quit();
}

int main ( int argc, char** argv )
{
	using namespace std;

	try
	{
		mainapp();
	}

	catch(GLExcep& e)
	{
		cout << e.title() << endl << e.what() << endl;
	}

	return 0;
}
