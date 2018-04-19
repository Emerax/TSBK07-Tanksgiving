#ifndef COLLISIONS_H
#define COLLISIONS_H
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

bool collisionsTest(vec3 *pos, GLfloat radius, Model *t);

#endif // COLLISIONS_H
