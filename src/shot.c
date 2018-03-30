#include "shot.h"

Model *model;
GLuint program;

float maxDist = 100.0f;
float speed = 0.1; 

void initShots(GLuint shotProgram) {
	model = LoadModelPlus("../assets/groundsphere.obj");
	program = shotProgram;
}

struct Shot spawnShot(vec3 pos, vec3 dir) {
	struct Shot s = {pos, dir, 0, maxDist};
	return s;
}

int updateShot(struct Shot *s, mat4 camMatrix) {
	// Update position, check if reached max
	s->pos = VectorAdd(s->pos, ScalarMult(s->dir, speed));
	s->distTravelled += speed;
	if (s->distTravelled > maxDist) return -1;
		
	glUseProgram(program);
	mat4 shotPosMat = T(s->pos.x, s->pos.y, s->pos.z);
	mat4 total = Mult(camMatrix, shotPosMat);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	DrawModel(model, program, "inPosition", "inNormal", "inTexCoord");	
	
	return 0;
}
