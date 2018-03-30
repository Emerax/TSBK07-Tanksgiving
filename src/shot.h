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

struct Shot {
	vec3 pos;
	vec3 dir;
	float distTravelled;
	float maxDist;
};

void initShots(GLuint shotProgram);
struct Shot spawnShot(vec3 pos, vec3 dir);
int updateShot(struct Shot *s, mat4 camMatrix);
