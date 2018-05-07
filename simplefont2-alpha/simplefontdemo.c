/* simplefontdemo.c */
// By Ingemar Ragnemalm 2009
// Slightly updated for 3.2 version.

// gcc simplefontdemo.c -o simplefontdemo simplefont.c -I common -I common/Linux/ common/*.c common/Linux/MicroGlut.c -DGL_GLEXT_PROTOTYPES -lGL -lX11 -lm


#ifdef __APPLE__
	#include <OpenGL/gl3.h>
//uses framework Cocoa
#else
	#include <GL/gl.h>
#endif
#include "MicroGlut.h"
#include <stdlib.h>
#include <string.h>
#include "simplefont.h"

float myTime = 0;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	sfDrawString(100, 100, "abcdefghijklmnopqrstuvwxyz1234567890:;><=?");
	sfDrawString(100, 120, " !""#$%:;><=? '*+-.,()/&&");
	sfDrawString(20, 20, "Hey Leroy!");
	sfDrawString(-20, -20, "Your Mama's calling!");
	
	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	// Viewport is a separate setting
	glViewport(0, 0, w, h);
	sfSetRasterSize(w, h);
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key)
   {
      case 27:
         exit(0);
   }
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(40,50);
	glutInitWindowSize(600,200);
	glutInitContextVersion(3, 2);
	glutCreateWindow("simplefont demo");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	
	sfMakeRasterFont(); // init font
	
	glutMainLoop();
}
