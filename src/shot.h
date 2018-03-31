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

typedef struct {
	vec3 pos;
	vec3 dir;
	float distTravelled;
	float maxDist;
} Shot;

void initShots(GLuint shotProgram);
Shot spawnShot(vec3 pos, vec3 dir);
int updateShot(Shot *s, mat4 camMatrix);
