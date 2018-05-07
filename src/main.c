// Lab 4, terrain generation

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
#include "simplefont.h"

#include "terrain.c"
#include "skybox.c"
#include "falling_snow.c"
#include "shot.c"
#include "collisions.c"
#include "target.c"

#include <stdlib.h>
#include <stdio.h>

mat4 projectionMatrix;

// vertex array object
Model *m, *m2, *tm;
// Reference to shader program
GLuint terrainShader;
GLuint tex1, tex2;
TextureData ttex; // terrain

// Tank shader stuff - change this to a more general "object" shader?
GLuint tankShader;
void displayTank(mat4);
void tankControls(mat4*);

// Tank models
Model *tankBase, *tankTower;

float moveSpeed = 0.5f;
vec3 tankPos = {1, 0, 1};
float rotSpeed = 0.1f;
float tankRot = 0;
float towerRot = 0;
int cooldown = 25;
int cdCounter = 0;

float camDistToTank = 10.0f;

// Skybox stuff
GLuint skyboxProgram;
GLuint skyboxTexture;
Model *skyboxModel;

//Special snowflake variables
GLuint snowflakeProgram;
GLuint snowflakeTexture;

Shot **shots;
Target **targets;

// Text display
void displayText();
int points;

void init(void) {
	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	printError("GL inits");

	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 50.0);

	// TODO: Change "terrain.frag" to a more general fragment shader that can be used by every object
	// Remember to change the name in the loadShaders function calls

	// Load and compile shader
	terrainShader = loadShaders("terrain.vert", "terrain.frag");
	glUseProgram(terrainShader);
	printError("init shader");

	glUniformMatrix4fv(glGetUniformLocation(terrainShader, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(terrainShader, "tex"), 0); // Texture unit 0
	LoadTGATextureSimple("../assets/Snow.tga", &tex1);

	// Load tank shader
	tankShader = loadShaders("tank.vert", "terrain.frag");
	glUseProgram(tankShader);
	glUniformMatrix4fv(glGetUniformLocation(tankShader, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

	// Load terrain data
	glUseProgram(terrainShader);
	LoadTGATextureData("../assets/fft-terrain.tga", &ttex);
	tm = GenerateTerrain(&ttex);
	printError("init terrain");

	// Load tank models
	// TODO: Load actual tank models, not just random shapes
	tankBase = LoadModelPlus("../assets/groundsphere.obj");
	tankTower = LoadModelPlus("../assets/octagon.obj");

	skyboxProgram = initSkybox(&skyboxModel, &skyboxTexture, projectionMatrix);

	//Let it snow
	snowflakeProgram = loadShaders("snowflake.vert", "snowflake.frag");
	initSnowflakes(snowflakeProgram, snowflakeTexture);

	initShots(tankShader, shots);
	initTargets(tankShader, targets);
	// Place target
	vec3 targetPos = {0,1,0};
	placeTarget(targetPos);

	sfMakeRasterFont();	
}

void display(void) {
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 mdlMatrix, camMatrix;

	printError("pre display");

	tankControls(&camMatrix);

	displaySkybox(camMatrix, skyboxProgram, skyboxTexture, skyboxModel);

	/* NOTE: The tankControls method modifies the camMatrix, so this method should
	   be called before uploading camMatrix to GPU */
	displayTank(camMatrix);

	glUseProgram(terrainShader);
	mdlMatrix = IdentityMatrix();
	glUniformMatrix4fv(glGetUniformLocation(terrainShader, "mdlMatrix"), 1, GL_TRUE, mdlMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(terrainShader, "camMatrix"), 1, GL_TRUE, camMatrix.m);

	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	DrawModel(tm, terrainShader, "inPosition", "inNormal", "inTexCoord");

	updateAllShots(camMatrix);
	displayTargets(camMatrix);

	points += checkCollisions(shots, targets);

	//Snowflakes need camera position to rotate properly.
	vec3 camPos = {tankPos.x - camDistToTank * cos(tankRot),
		tankPos.y + 4,
		tankPos.z - camDistToTank * sin(tankRot)};
	displaySnowflakes(camPos, projectionMatrix, camMatrix);
	
	displayText();

	printError("display 2");

	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h) {
	glViewport(0, 0, w, h);
	sfSetRasterSize(w, h);
}

void tankControls(mat4 *camMatrix) {
	// Manage keyboard controls
	if (glutKeyIsDown('w')) {
		tankPos.x += moveSpeed * cos(tankRot);
		tankPos.z += moveSpeed * sin(tankRot);
	}
	if (glutKeyIsDown('s')) {
		tankPos.x -= moveSpeed * cos(tankRot);
		tankPos.z -= moveSpeed * sin(tankRot);
	}
	if (glutKeyIsDown('d')) {
		tankRot += rotSpeed;
	}
	if (glutKeyIsDown('a')) {
		tankRot -= rotSpeed;
	}
	/* Debug flight controls
	TODO: Remove when terrain following is to be implemented */
	if(glutKeyIsDown('q')){
		tankPos.y += moveSpeed;
	}
	if(glutKeyIsDown('e')){
		tankPos.y -= moveSpeed;
	}

	tankPos.y = getHeight(tankPos.x, tankPos.z, tm, &ttex);

	vec3 camPos = {tankPos.x - camDistToTank * cos(tankRot),
					tankPos.y + 4,
					tankPos.z - camDistToTank * sin(tankRot)};

	*camMatrix = lookAt(camPos.x, camPos.y, camPos.z,
		tankPos.x, tankPos.y, tankPos.z,
		0.0, 1.0, 0.0);

	// Rotate the tower
	if (glutKeyIsDown('j')) {
		towerRot -= rotSpeed;
	}
	if (glutKeyIsDown('k')) {
		towerRot += rotSpeed;
	}
	if (glutKeyIsDown(' ')) {
		if (cdCounter == 0) {
			vec3 dir = {cos(towerRot), 0, sin(towerRot)};
			spawnShot(tankPos, dir);
			cdCounter = cooldown;
		}
	}
	if (cdCounter != 0) cdCounter--;
}

void displayTank(mat4 camMatrix) {

	GLint prevShader;
	glGetIntegerv(GL_CURRENT_PROGRAM, &prevShader);
	// Draw tank base
	glUseProgram(tankShader);
	mat4 tankPosMat = T(tankPos.x, tankPos.y, tankPos.z);
	mat4 rotMat = Ry(-tankRot);
	mat4 total = Mult(tankPosMat, rotMat);
	glUniformMatrix4fv(glGetUniformLocation(tankShader, "mdlMatrix"), 1, GL_TRUE, total.m);
	glUniformMatrix4fv(glGetUniformLocation(tankShader, "camMatrix"), 1, GL_TRUE, camMatrix.m);
	DrawModel(tankBase, tankShader, "inPosition", "inNormal", "inTexCoord");

	// Draw tank tower
	// TODO: tankPos.y + 1 is only a placeholder, change this when a real model is used
	mat4 towerPosMat = T(tankPos.x, tankPos.y + 1, tankPos.z);
	mat4 towerRotMat = Ry(-towerRot);
	mat4 towerTotal = Mult(towerPosMat, towerRotMat);
	glUniformMatrix4fv(glGetUniformLocation(tankShader, "mdlMatrix"), 1, GL_TRUE, towerTotal.m);
	DrawModel(tankTower, tankShader, "inPosition", "inNormal", "inTexCoord");

	// Restore the previous shader
	glUseProgram(prevShader);

}

void displayText() {
	char pointString[4];
	snprintf(pointString, 4, "%d", points);
	char text[12];
	strcpy(text, "Points: ");
	strcat(text, pointString);	

	sfDrawString(50, 50, text);
}

void timer(int i) {
	glutTimerFunc(20, &timer, i);
	glutPostRedisplay();
}

void mouse(int x, int y) {
	//printf("%d %d\n", x, y);
}

int main(int argc, char **argv) {
	shots = calloc(maxShots, sizeof(Shot*));
	targets = calloc(maxTargets, sizeof(Target*));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (800, 800);
	glutCreateWindow ("TSBK07 Lab 4");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	init ();
	glutTimerFunc(20, &timer, 0);
	glutPassiveMotionFunc(mouse);

	glutMainLoop();
	exit(0);
}
