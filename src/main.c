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
Model *tankBase, *tankTower, *nose;
GLuint noseTex;

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

/*Used by shots to communicate to snowflakes when to spawn snosplosions.
	(-1, -1, -1) is always outside the map, and can therefore be used to check if
	the vec3 has changed.*/
vec3 snosplosionPos = {-1, -1, -1};

// Ojbective arrow thingey
Model *arrow;
void displayArrow(mat4);

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
	tankTower = LoadModelPlus("../assets/groundsphere.obj");
	nose = LoadModelPlus("../assets/nose.obj");
	glUniform1i(glGetUniformLocation(tankShader, "tex"), 0); // Texture unit 1
	LoadTGATextureSimple("../assets/nose.tga", &noseTex);

	skyboxProgram = initSkybox(&skyboxModel, &skyboxTexture, projectionMatrix);

	//Let it snow
	snowflakeProgram = loadShaders("snowflake.vert", "snowflake.frag");
	initSnowflakes(snowflakeProgram, snowflakeTexture);

	initShots(tankShader, shots);
	initTargets(tankShader, targets);

	arrow = LoadModelPlus("../assets/nose.obj");

	// Place target
	placeRandomTarget(tm, &ttex);

	sfMakeRasterFont();
}

void display(void) {
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 mdlMatrix, camMatrix;

	printError("pre display");

	/* NOTE: The tankControls method modifies the camMatrix, so this method should
	be called before uploading camMatrix to GPU */
	tankControls(&camMatrix);

	displaySkybox(camMatrix, skyboxProgram, skyboxTexture, skyboxModel);
	displayArrow(camMatrix);

	displayTank(camMatrix);

	glUseProgram(terrainShader);
	mdlMatrix = IdentityMatrix();
	glUniformMatrix4fv(glGetUniformLocation(terrainShader, "mdlMatrix"), 1, GL_TRUE, mdlMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(terrainShader, "camMatrix"), 1, GL_TRUE, camMatrix.m);

	glUniform1i(glGetUniformLocation(terrainShader, "tex"), 0); // Texture unit 1
	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	DrawModel(tm, terrainShader, "inPosition", "inNormal", "inTexCoord");

	updateAllShots(camMatrix);
	displayTargets(camMatrix);

	int pts = checkCollisions(shots, targets, &snosplosionPos);
	int p;
	for (p = 0; p < pts; ++p) {
		placeRandomTarget(tm, &ttex);
	}
	points += pts;

	if(snosplosionPos.x != -1 && snosplosionPos.y != -1 && snosplosionPos.z != -1){
		//A collision has been reported, spawn the snosplosion!
		spawnSnosplosion(snosplosionPos, 8000, 5);
		//Reset values so we don't keep spawning the same snosplosion.
		snosplosionPos.x = -1;
		snosplosionPos.y = -1;
		snosplosionPos.z = -1;
	}

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
			vec3 dir = {-sin(towerRot), 0, cos(towerRot)};
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

	// TODO: No texture is applied to snowmanbody, FIX THIS
	glUniform1i(glGetUniformLocation(tankShader, "tex"), 0);
	glBindTexture(GL_TEXTURE_2D, tex1);
	mat4 tankPosMat = T(tankPos.x, tankPos.y, tankPos.z);
	mat4 rotMat = Ry(-tankRot);
	mat4 total = Mult(tankPosMat, rotMat);
	glUniformMatrix4fv(glGetUniformLocation(tankShader, "mdlMatrix"), 1, GL_TRUE, total.m);
	glUniformMatrix4fv(glGetUniformLocation(tankShader, "camMatrix"), 1, GL_TRUE, camMatrix.m);
	DrawModel(tankBase, tankShader, "inPosition", "inNormal", "inTexCoord");

	// Draw tank tower
	float towerScale = 0.7;
	mat4 towerPosMat = T(tankPos.x, tankPos.y + 1.5, tankPos.z);
	mat4 towerRotMat = Ry(-towerRot);
	mat4 towerScaleMat = S(towerScale, towerScale, towerScale);
	mat4 towerTotal = Mult(towerPosMat, Mult(towerRotMat, towerScaleMat));
	glUniformMatrix4fv(glGetUniformLocation(tankShader, "mdlMatrix"), 1, GL_TRUE, towerTotal.m);
	DrawModel(tankTower, tankShader, "inPosition", "inNormal", "inTexCoord");

	// Draw nose
	mat4 nosePosMat = T(0, 0.8, 0.85); // Position relative to head
	mat4 noseTotal = Mult(towerPosMat, Mult(towerRotMat, nosePosMat));
	glUniformMatrix4fv(glGetUniformLocation(tankShader, "mdlMatrix"), 1, GL_TRUE, noseTotal.m);
	glUniform1i(glGetUniformLocation(tankShader, "tex"), 0); // Texture unit 0
	glBindTexture(GL_TEXTURE_2D, noseTex);
	DrawModel(nose, tankShader, "inPosition", "inNormal", "inTexCoord");

	// Restore the previous shader
	glUseProgram(prevShader);

}

void displayArrow(mat4 camMatrix) {
	glUseProgram(tankShader);
	glDisable(GL_CULL_FACE);
	// Use the same method of calculating the position of the camera,
	// but use half the distance to the player. Simple and works well!
	vec3 arrowPos = {tankPos.x - (camDistToTank/2) * cos(tankRot),
					tankPos.y + 4,
					tankPos.z - (camDistToTank/2) * sin(tankRot)};
	mat4 pos = T(arrowPos.x, arrowPos.y, arrowPos.z);

	vec3 nextTargetPos = targets[nextTargetIdx]->pos;
	vec3 dir = Normalize(VectorSub(nextTargetPos, arrowPos));
	float rotY = atan2(dir.x, dir.z);
	mat4 rotYMat = Ry(rotY);

	mat4 total = Mult(pos, rotYMat);

	glUniformMatrix4fv(glGetUniformLocation(tankShader, "mdlMatrix"), 1, GL_TRUE, total.m);
	glUniformMatrix4fv(glGetUniformLocation(tankShader, "camMatrix"), 1, GL_TRUE, camMatrix.m);
	DrawModel(arrow, tankShader, "inPosition", "inNormal", "inTexCoord");

	glEnable(GL_CULL_FACE);

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
