#include "target.h"

#define maxTargets 10

Model *model;
GLuint program;

Target **targets;
int idx = 0;

void initTargets(GLuint targetProgram) {
	model = LoadModelPlus("../assets/groundsphere.obj");
	program = targetProgram; // Consider loading shaders here?
	targets = calloc(maxTargets, sizeof(Target*));
	int i;
	for (i = 0; i < maxTargets; ++i) 
		targets[i] = NULL;
}

void placeTarget(vec3 pos) {
	Target* t = malloc(sizeof(Target));
	t->pos = pos;
	t->idx = idx;
	if (targets[idx] != NULL)
		deleteTarget(targets[idx]);
	targets[idx] = t;
	if (idx < maxTargets - 1) idx++;
	else idx = 0;
	
}

void displayTargets(mat4 camMatrix) {
	int i;
	for (i = 0; i < maxTargets; ++i) {
		Target* t = targets[i];
		if (t != NULL) {
			glUseProgram(program);
			mat4 posMat = T(t->pos.x, t->pos.y, t->pos.z);
			mat4 total = Mult(camMatrix, posMat);
			glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
			DrawModel(model, program, "inPosition", "inNormal", "inTexCoord");
		}
	} 
}

void deleteTarget(Target *t) {
	targets[t->idx] = NULL;
	free(t);
}
