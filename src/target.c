#include "target.h"

#include "terrain.h"
#include <time.h>

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
	srand(time(NULL));
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

void placeRandomTarget(Model* tm, TextureData* tex) {
	int w = tex->width;
	int h = tex->height;

	w = 10;
	h = 10;

	int x = rand() % w;
	while (x >= w) x = rand() % w;
	int z = rand() % h;
	while (z >= h) z = rand() % h;
	float y = getHeight(x, z, tm, tex) + 1.5;
	
	printf("Placed target at %d %d\n", x, z);	

	vec3 pos = {x, y, z};
	placeTarget(pos);
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
