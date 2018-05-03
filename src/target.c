#include "target.h"

Model *model;
GLuint targetShader;

int idx = 0;

Target **targets;

void initTargets(GLuint targetProgram, Target **targetArray) {
	model = LoadModelPlus("../assets/groundsphere.obj");
	targetShader = targetProgram; // Consider loading shaders here?
	targets = targetArray;
	int i;
	for (i = 0; i < maxTargets; ++i) {
		targets[i] = NULL;
	}
}

void placeTarget(vec3 pos) {
	Target* t = malloc(sizeof(Target));
	t->pos = pos;
	t->idx = idx;
	
	if (targets[idx] != NULL) {
		deleteTarget(targets[idx]);
	}	
	targets[idx] = t;
	if (idx < maxTargets - 1) idx++;
	else idx = 0;
	
}

void displayTargets(mat4 camMatrix) {
	int i;
	for (i = 0; i < maxTargets; ++i) {
		Target* t = targets[i];
		if (t != NULL) {
			GLint prevShader;
			glGetIntegerv(GL_CURRENT_PROGRAM, &prevShader);
			glUseProgram(targetShader);
			mat4 posMat = T(t->pos.x, t->pos.y, t->pos.z);
			mat4 total = posMat;
			glUniformMatrix4fv(glGetUniformLocation(targetShader, "mdlMatrix"), 1, GL_TRUE, total.m);
			DrawModel(model, targetShader, "inPosition", "inNormal", "inTexCoord");

			glUseProgram(prevShader);
		}
	} 
}

void deleteTarget(Target *t) {
	targets[t->idx] = NULL;
	free(t);
}

Target*** getTargets() {
	return &targets;
}
