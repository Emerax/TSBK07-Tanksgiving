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

// Extremely simple collision detection that assumes spherical objects
bool sphereCollisionTest(vec3 *pos1, GLfloat radius1, vec3 *pos2, GLfloat radius2);
// This function still assums that the target model in question is situated at (0,0,0)
bool collisionsTest(vec3 *pos, GLfloat radius, Model *t);

#endif // COLLISIONS_H
