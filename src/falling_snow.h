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

struct snowflake {
	float x, y, z;
	bool active;
};

// Used mostly as a container for coordinates.
typedef struct vec2 {
	GLfloat x, z;
} vec2;

void initSnowflakes(GLuint program, GLuint texture);

void displaySnowflakes(vec3 camPos, mat4 projectionMatrix, mat4 viewMatrix);

struct snowflake createSnowflake(float x, float y, float z, bool active);

vec2 randomUnitCircle(float r);

float randomFloat(float max);
