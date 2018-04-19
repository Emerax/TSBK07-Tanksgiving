#include "collisions.h"

const GLfloat SMALL_NUMBER = -100000000;

bool collisionsTest(vec3 *pos, GLfloat radius, Model *target) {
	int i; 
	vec3 p0, p1, p2, v1, v2, n;
	GLfloat diff;

	GLfloat bestDiff = SMALL_NUMBER;
	vec3 bestMove, bestNormal;

	for (i = 0; i < target->numIndices - 1; i += 3) {
		p0 = (*(vec3 *)(&target->vertexArray[target->indexArray[i]*3]));
		p1 = (*(vec3 *)(&target->vertexArray[target->indexArray[i+1]*3]));
		p2 = (*(vec3 *)(&target->vertexArray[target->indexArray[i+2]*3]));
		v1 = VectorSub(p1, p0);
		v2 = VectorSub(p2, p0);
		n = Normalize(CrossProduct(v1, v2));

		diff = -DotProduct(n, p0) - radius + DotProduct(n, *pos);
		return true;
	}
	return false;
}
