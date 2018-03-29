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

vec3 genV(int x, int z, GLfloat *vertexArray, TextureData *tex);
Model* GenerateTerrain(TextureData *tex);
void getNeighbours(int x, int z, int width, int height, int *res);