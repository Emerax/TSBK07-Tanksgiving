#include "shot.h"
#include "target.h"
#include "collisions.h"

#define maxShots 100

Model *model;
GLuint program;

float maxDist = 100.0f;
float speed = 1;
float shotScale = 0.2;

int shotIdx = 0;

vec3 shotOffset = {0, 2, 0}; // Offset so that the shots don't appear at the player's feet

void initShots(GLuint shotProgram, Shot** shotArray) {
	model = LoadModelPlus("../assets/groundsphere.obj");
	program = shotProgram;
	shots = shotArray;
	int i;
	for (i = 0; i < maxShots; ++i) {
		shots[i] = NULL;
	}
}

Shot* spawnShot(vec3 pos, vec3 dir) {
	Shot* s = malloc(sizeof(Shot));
	s->pos = VectorAdd(pos, shotOffset);
	s->dir = dir;
	s->distTravelled = 0;
	s->maxDist = maxDist;
	s->idx = shotIdx;
	if (shots[shotIdx] != NULL) { 
		deleteShot(shots[shotIdx]);
	}
	shots[shotIdx] = s;
	if (shotIdx < maxShots - 1) shotIdx++;
	else shotIdx = 0;
	return s;
}

void updateAllShots(mat4 camMatrix) {
	int i,j;
	for (i = 0; i < maxShots; i++) {
		if (shots[i] != NULL)
			updateShot(shots[i], camMatrix);
	}
}

int updateShot(Shot *s, mat4 camMatrix) {
	s->pos = VectorAdd(s->pos, ScalarMult(s->dir, speed));
	s->distTravelled += speed;
	if (s->distTravelled > maxDist) {
		deleteShot(s);
		return -1;
	}
	glUseProgram(program);
	mat4 shotPosMat = T(s->pos.x, s->pos.y, s->pos.z);
	mat4 scale = S(shotScale, shotScale, shotScale);
	mat4 total = Mult(camMatrix, Mult(shotPosMat, scale));
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	DrawModel(model, program, "inPosition", "inNormal", "inTexCoord");	
	
	return 0;
}

void deleteShot(Shot *s) {
	shots[s->idx] = NULL;
	free(s);	
}

