#include "shot.h"

#define maxShots 100

Model *model;
GLuint program;

float maxDist = 100.0f;
float speed = 1;

Shot** shots; 
int shotIdx = 0;

void initShots(GLuint shotProgram) {
	model = LoadModelPlus("../assets/groundsphere.obj");
	program = shotProgram;
	shots = calloc(maxShots, sizeof(Shot*));
	int i;
	for (i = 0; i < maxShots; ++i) {
		shots[i] = NULL;
	}
}

// TODO: Fix issue where the shots that are created are not 
// still there when updating. Probably because s falls out of scope?
Shot* spawnShot(vec3 pos, vec3 dir) {
	Shot* s = malloc(sizeof(Shot));
	s->pos = pos;
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
	int i;
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
	mat4 total = Mult(camMatrix, shotPosMat);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	DrawModel(model, program, "inPosition", "inNormal", "inTexCoord");	
	
	return 0;
}

void deleteShot(Shot *s) {
	shots[s->idx] = NULL;
	free(s);	
}

