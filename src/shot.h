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

typedef struct Shot {
	vec3 pos;
	vec3 dir;
	float distTravelled;
	float maxDist;
	int idx;
};

typedef struct Shot Shot;
// FISK

void initShots(GLuint shotProgram);
Shot *spawnShot(vec3 pos, vec3 dir);
int updateShot(Shot *s, mat4 camMatrix);
void deleteShot(Shot *s);
