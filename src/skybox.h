#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"

GLuint initSkybox(Model** skyboxModel, GLuint *skyboxTexture, mat4 projectionMatrix);
void displaySkybox(mat4 camMatrix, GLuint skyboxProgram, GLuint skyboxTexture, Model *skyboxModel);
