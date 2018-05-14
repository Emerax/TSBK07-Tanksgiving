#include "collisions.h"
#include "shot.h"
#include "target.h"

const GLfloat SMALL_NUMBER = -100000000;

bool sphereCollisionTest(vec3 *pos1, GLfloat radius1, vec3 *pos2, GLfloat radius2) {
	vec3 diff = VectorSub(*pos2, *pos1);
	return (Norm(diff) <= radius1 + radius2);
}

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

int checkCollisions(Shot** shots, Target** targets, vec3* collisionPos) {
	int i, j;
	int points = 0;
	for (i = 0; i < maxShots; i++) {
		Shot* shot = shots[i];
		if (shot != NULL) {
			for (j = 0; j < maxTargets; j++) {
				Target* target = targets[j];
				if (target != NULL) {
					// Offset positions to center of objects
					vec3 sPos = shot->pos;
					sPos.y += shotScale;
					vec3 tPos = target->pos;
					tPos.y += 1;
					if (sphereCollisionTest(&sPos, shotScale, &tPos, 1)) {
						//Update collisionPos, this tells the particle system to spawn a snosplosion here later.
						collisionPos->x = sPos.x;
						collisionPos->y = sPos.y;
						collisionPos->z = sPos.z;
						deleteShot(shot);
						deleteTarget(target);
						points++;
					}
				}
			}
		}
	}
	return points;
}
