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

mat4 projectionMatrix;

Model* GenerateTerrain(TextureData *tex) {
	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width-1) * (tex->height-1) * 2;
	int x, z;

	GLfloat *vertexArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = malloc(sizeof(GLuint) * triangleCount*3);

	printf("bpp %d\n", tex->bpp);
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
			// Vertex array. You need to scale this properly
			vertexArray[(x + z * tex->width)*3 + 0] = x / 1.0;
			vertexArray[(x + z * tex->width)*3 + 1] = tex->imageData[(x + z * tex->width) * (tex->bpp/8)] / 100.0;
			vertexArray[(x + z * tex->width)*3 + 2] = z / 1.0;
			// Normal vectors. You need to calculate these.
			normalArray[(x + z * tex->width)*3 + 0] = 0.0;
			normalArray[(x + z * tex->width)*3 + 1] = 1.0;
			normalArray[(x + z * tex->width)*3 + 2] = 0.0;
			// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * tex->width)*2 + 0] = x; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width)*2 + 1] = z; // (float)z / tex->height;
		}
		for (x = 0; x < tex->width-1; x++)
		for (z = 0; z < tex->height-1; z++)
		{
			// Triangle 1
			indexArray[(x + z * (tex->width-1))*6 + 0] = x + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 1] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 2] = x+1 + z * tex->width;
			// Triangle 2
			indexArray[(x + z * (tex->width-1))*6 + 3] = x+1 + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 4] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 5] = x+1 + (z+1) * tex->width;
		}

		// End of terrain generation

		// Create Model and upload to GPU:

		Model* model = LoadDataToModel(
			vertexArray,
			normalArray,
			texCoordArray,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3);

			return model;
}


// vertex array object
Model *m, *m2, *tm;
// Reference to shader program
GLuint program;
GLuint tex1, tex2;
TextureData ttex; // terrain

// Tank shader stuff - change this to a more general "object" shader?
GLuint tankShader;
void drawTank(mat4);
void tankControls(mat4*);

// Tank models
Model *tankBase, *tankTower;

float moveSpeed = 0.5f;
vec3 tankPos = {1, 0, 1};
float rotSpeed = 0.1f;
float tankRot = 0;

float camDistToTank = 10.0f;

// Skybox stuff
GLuint skyboxProgram;
GLuint skyboxTexture;
Model *skyboxModel;

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
	program = loadShaders("terrain.vert", "terrain.frag");
	glUseProgram(program);
	printError("init shader");

	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(program, "tex"), 0); // Texture unit 0
	LoadTGATextureSimple("../assets/maskros512.tga", &tex1);

	// Load tank shader
	tankShader = loadShaders("tank.vert", "terrain.frag");
	glUseProgram(tankShader);
	glUniformMatrix4fv(glGetUniformLocation(tankShader, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

	// Load terrain data

	LoadTGATextureData("../assets/big-flat-terrain.tga", &ttex);
	tm = GenerateTerrain(&ttex);
	printError("init terrain");

	// Load tank models
	// TODO: Load actual tank models, not just random shapes
	tankBase = LoadModelPlus("../assets/groundsphere.obj");
	tankTower = LoadModelPlus("../assets/octagon.obj");

	// Skybox initialization
	skyboxProgram = loadShaders("skybox.vert", "skybox.frag");
	glUseProgram(skyboxProgram);
	glUniform1i(glGetUniformLocation(skyboxProgram, "texUnit"), 0);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	skyboxModel = LoadModelPlus("../assets/skybox.obj");
	
	LoadTGATextureSimple("../assets/SkyBox512.tga", &skyboxTexture);
	
}

void display(void) {
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 total, modelView, camMatrix;

	printError("pre display");
		
	tankControls(&camMatrix);

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

	mat4 skyboxOffset = T(0, -2, 0);

	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "camMatrix"), 1, GL_TRUE, skyboxMat.m);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "mdlMatrix"), 1, GL_TRUE, skyboxOffset.m);
	DrawModel(skyboxModel, skyboxProgram, "inPosition", "inNormal", "inTexCoord");

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(program);

	/* NOTE: The tankControls method modifies the camMatrix, so this method should
	   be called before uploading camMatrix to GPU */
	drawTank(camMatrix);
	modelView = IdentityMatrix();
	total = Mult(camMatrix, modelView);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);

	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");

	printError("display 2");

	glutSwapBuffers();
}

// TODO: Add tank tower support - load the object, place it on top of base, rotate it freely
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

	vec3 camPos = {tankPos.x - camDistToTank * cos(tankRot), tankPos.y + 4, tankPos.z - camDistToTank * sin(tankRot)};

	*camMatrix = lookAt(camPos.x, camPos.y, camPos.z,
		tankPos.x, tankPos.y, tankPos.z,
		0.0, 1.0, 0.0);

}

void drawTank(mat4 camMatrix) {	
	// Upload to the GPU
	glUseProgram(tankShader);
	mat4 tankPosMat = T(tankPos.x, tankPos.y, tankPos.z);
	mat4 rotMat = Ry(-tankRot);
	mat4 total = Mult(camMatrix, Mult(tankPosMat, rotMat));
	glUniformMatrix4fv(glGetUniformLocation(tankShader, "mdlMatrix"), 1, GL_TRUE, total.m);
	DrawModel(tankBase, tankShader, "inPosition", "inNormal", "inTexCoord");
	glUseProgram(program);
}

void timer(int i) {
	glutTimerFunc(20, &timer, i);
	glutPostRedisplay();
}

void mouse(int x, int y) {
	//printf("%d %d\n", x, y);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (800, 800);
	glutCreateWindow ("TSBK07 Lab 4");
	glutDisplayFunc(display);
	init ();
	glutTimerFunc(20, &timer, 0);

	glutPassiveMotionFunc(mouse);

	glutMainLoop();
	exit(0);
}
