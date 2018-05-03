#ifndef SHOT_H
#define SHOT_H
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
#include "target.h"

typedef struct {
	vec3 pos;
	vec3 dir;
	float distTravelled;
	float maxDist;
	int idx;
} Shot;

Shot** shots; 

void initShots(GLuint shotProgram, Shot**);
Shot *spawnShot(vec3 pos, vec3 dir);
void updateAllShots(mat4 camMatrix);
int updateShot(Shot *s, mat4 camMatrix);
void deleteShot(Shot *s);

#endif // SHOT_H
