/* simplefontdemo2.c */
// By Ingemar Ragnemalm 20??
// Updated for SimpleFont 2 with support for external texture fonts.

// WARNING: SimpleFont 2 has had severe bugs showing on CentOS. Now (2018) I found that it works quite well on Linux Mint, so I decided to release it... with a fair warning! Alpha software!

// gcc simplefontdemo2.c -o simplefontdemo2 simplefont.c -I common -I common/Linux/ common/*.c common/Linux/MicroGlut.c -DGL_GLEXT_PROTOTYPES -lGL -lX11 -lm

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
#include "LoadTGA.h"

float myTime = 0;

GLuint font1, font2, font3, font4;

void display()
{
	glClearColor(0.6, 0.5, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	sfSetFont(font1);
	sfSetFontColor(0, 1, 1);
	sfDrawString(100, 200, "abcdefghijklmnopqrstuvwxyz1234567890:;><=?");
	sfSetFontColor(1, 0, 0.5);
	sfDrawString(100, 230, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	sfSetFontColor(1, 1, 0);
	sfDrawString(100, 260, " !\"#$%:;><=? '*+-.,()/&&");
	sfSetFont(font3);
	sfSetFontColor(1, 0, 0);
	sfDrawString(20, 60, "Hey Leroy!");
	sfSetFont(font2);
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
	TextureData texture;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(40,50);
	glutInitWindowSize(800,400);
	glutInitContextVersion(3, 2);
	glutCreateWindow("simplefont demo");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	font1 = sfMakeRasterFont(); // init font
	
	// Support transparent fonts
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	LoadTGATextureData("monaco-black.tga", &texture);
	font2 = sfLoadExternalFont(texture.imageData, 14.4, 32, texture.width, texture.height, 0);

// FUNKAR!
	LoadTGATextureData("TexturfonttestA.tga", &texture);
//	font3 = sfLoadExternalFont(texture.imageData, 28.9, 57, texture.width, texture.height, 1);
	font3 = sfLoadExternalFont(texture.imageData, 28.9, 57.6, texture.width, texture.height, 1);
	LoadTGATextureData("Texturfonttest256.tga", &texture);
//	font2 = sfLoadExternalFont(texture.imageData, 14.95, 28.5, texture.width, texture.height, 1);
	font4 = sfLoadExternalFont(texture.imageData, 14.45, 28.75, texture.width, texture.height, 1);
	
	glutMainLoop();
	exit(0);
}
