#include "skybox.h"

GLuint initSkybox(Model** skyboxModel, GLuint *skyboxTexture, mat4 projectionMatrix) {
	GLuint skyboxProgram = loadShaders("skybox.vert", "skybox.frag");
	glUseProgram(skyboxProgram);
	glUniform1i(glGetUniformLocation(skyboxProgram, "texUnit"), 0);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	*skyboxModel = LoadModelPlus("../assets/skybox.obj");
	
	LoadTGATextureSimple("../assets/SkyBox512.tga", skyboxTexture);

	return skyboxProgram;
}

void displaySkybox(mat4 camMatrix, GLuint skyboxProgram, GLuint skyboxTexture, Model *skyboxModel) {
	// Skybox display
	glUseProgram(skyboxProgram);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skyboxTexture); 

	mat4 skyboxMat = camMatrix;
	skyboxMat.m[3] = 0;
	skyboxMat.m[7] = 0;
	skyboxMat.m[11] = 0;

	mat4 skyboxOffset = T(0, -1, 0);

	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "camMatrix"), 1, GL_TRUE, skyboxMat.m);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "mdlMatrix"), 1, GL_TRUE, skyboxOffset.m);
	DrawModel(skyboxModel, skyboxProgram, "inPosition", "inNormal", "inTexCoord");

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
}
