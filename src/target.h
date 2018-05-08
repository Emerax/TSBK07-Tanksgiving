#ifndef TARGET_H
#define TARGET_H
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

#define maxTargets 10


typedef struct {
	vec3 pos;
	int idx;
} Target;

//extern Target** targets;

void initTargets(GLuint program, Target**);
void placeTarget(vec3 pos);
void placeRandomTarget(Model* tm, TextureData* tex);
void displayTargets(mat4 camMatrix);
void deleteTarget(Target*);
Target*** getTargets();

#endif // TARGET_H
