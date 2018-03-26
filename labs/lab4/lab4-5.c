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

mat4 keyboardControls();
void getNeighbours(int x, int z, int width, int height, int *res);
vec3 camPos = {0,5,0};
vec3 camDir = {0,0,1};
const static vec3 VEC3_UP = {0, 1, 0};

GLfloat moveSpeed = 0.1;
GLfloat rotSpeed = 0.1;

void ballControl(Model*, TextureData*, mat4);
Model* ballModel;
vec3 ballPos = {2,0,2};

vec3 genV(int x, int z, GLfloat *vertexArray, TextureData *tex) {
	vec3 res;
	res.x = vertexArray[(x + z * tex->width)*3 + 0];
	res.y = vertexArray[(x + z * tex->width)*3 + 1];
	res.z = vertexArray[(x + z * tex->width)*3 + 2];

	return res;
}

vec3 genVByIdx(int idx, GLfloat *vertexArray) {
	vec3 res;
	res.x = vertexArray[idx + 0];
	res.y = vertexArray[idx + 1];
	res.z = vertexArray[idx + 2];

	return res;
}

float getHeight(float x, float z, Model* terrain, TextureData* tex) {

	int floorX = floor(x);
	int floorZ = floor(z);

	vec3 p1, p2, p3;

	// Choose which of the polygons in this square to use.
	if (x - floorX + z - floorZ <= 1) {
	// Triangle 1
		p1.x = terrain->vertexArray[(floorX + floorZ * tex->width) * 3 + 0];
		p1.y = terrain->vertexArray[(floorX + floorZ * tex->width) * 3 + 1];
		p1.z = terrain->vertexArray[(floorX + floorZ * tex->width) * 3 + 2];

		p2.x = terrain->vertexArray[(floorX + 1 + floorZ * tex->width) * 3 + 0];
		p2.y = terrain->vertexArray[(floorX + 1 + floorZ * tex->width) * 3 + 1];
		p2.z = terrain->vertexArray[(floorX + 1 + floorZ * tex->width) * 3 + 2];

		p3.x = terrain->vertexArray[(floorX + (floorZ + 1) * tex->width) * 3 + 0];
		p3.y = terrain->vertexArray[(floorX + (floorZ + 1) * tex->width) * 3 + 1];
		p3.z = terrain->vertexArray[(floorX + (floorZ + 1) * tex->width) * 3 + 2];
	} else {
	// Triangle 2
		p1.x = terrain->vertexArray[(floorX + 1 + (floorZ + 1) * tex->width) * 3 + 0];
		p1.y = terrain->vertexArray[(floorX + 1 + (floorZ + 1) * tex->width) * 3 + 1];
		p1.z = terrain->vertexArray[(floorX + 1 + (floorZ + 1) * tex->width) * 3 + 2];

		p2.x = terrain->vertexArray[(floorX + 1 + floorZ * tex->width) * 3 + 0];
		p2.y = terrain->vertexArray[(floorX + 1 + floorZ * tex->width) * 3 + 1];
		p2.z = terrain->vertexArray[(floorX + 1 + floorZ * tex->width) * 3 + 2];

		p3.x = terrain->vertexArray[(floorX + (floorZ + 1) * tex->width) * 3 + 0];
		p3.y = terrain->vertexArray[(floorX + (floorZ + 1) * tex->width) * 3 + 1];
		p3.z = terrain->vertexArray[(floorX + (floorZ + 1) * tex->width) * 3 + 2];
		
	}

	vec3 normal = CrossProduct(VectorSub(p2, p1), VectorSub(p3, p1));
	if (normal.y == 0) {
		// We don't want division by 0, return something else
		return 0;
	} 
	float d = DotProduct(p1, normal);


	return ((d - normal.x * x - normal.z * z) / normal.y);

}

Model* GenerateTerrain(TextureData *tex)
{
	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width-1) * (tex->height-1) * 2;
	int x, z;

	GLfloat *vertexArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = malloc(sizeof(GLuint) * triangleCount*3);
	
	printf("bpp %d\n", tex->bpp);
	int n[6] = {0};
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
// Vertex array. You need to scale this properly
			vertexArray[(x + z * tex->width)*3 + 0] = x / 1.0;
			vertexArray[(x + z * tex->width)*3 + 1] = tex->imageData[(x + z * tex->width) * (tex->bpp/8)] / 10.0;
			vertexArray[(x + z * tex->width)*3 + 2] = z / 1.0;
			
// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * tex->width)*2 + 0] = x; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width)*2 + 1] = z; // (float)z / tex->height;
		}

	// Normal vectors
	for (x = 0; x < tex->width; x++) {
		for (z = 0; z < tex->height; z++) {
			// IDEA: Get 3 neighbour vertices, calculate normal using cross product of vectors between them. 
			// Created function below!
			getNeighbours(x, z, tex->width, tex->height, n);
			//for (int i = 0; i < 6; i+=2) {
			//	printf("%d, %d\n", neighbours[i], neighbours[i+1]);
			//}

			vec3 normal = CrossProduct(VectorSub(genV(n[2], n[3], vertexArray, tex), genV(n[0], n[1], vertexArray, tex)),
									   VectorSub(genV(n[4], n[5], vertexArray, tex), genV(n[0], n[1], vertexArray, tex)));

			//printf("Normal: %f, %f, %f\n", normal.x, normal.y, normal.z);

			normalArray[(x + z * tex->width)*3 + 0] = normal.x;
			normalArray[(x + z * tex->width)*3 + 1] = normal.y;
			normalArray[(x + z * tex->width)*3 + 2] = normal.z;
		}
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

GLuint objProgram;

void init(void)
{
	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 50.0);

	// Load and compile shader
	//objProgram = loadShaders("objects.vert", "objects.vert");
	program = loadShaders("terrain.vert", "terrain.frag");
	glUseProgram(program);
	printError("init shader");
	
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(program, "tex"), 0); // Texture unit 0
	LoadTGATextureSimple("maskros512.tga", &tex1);

// Load terrain data
	
	LoadTGATextureData("fft-terrain.tga", &ttex);
	tm = GenerateTerrain(&ttex);
	printError("init terrain");

// load baalllzzz
	ballModel = LoadModelPlus("groundsphere.obj");
}

void display(void)
{
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	mat4 total, modelView, camMatrix;
	
	printError("pre display");
	
	glUseProgram(program);

	// Build matrix
	
	camMatrix = keyboardControls();
	modelView = IdentityMatrix();
	total = Mult(camMatrix, modelView);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, camMatrix.m);
	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");

	ballControl(tm, &ttex, camMatrix);

	printError("display 2");
	
	glutSwapBuffers();
}

void timer(int i)
{
	glutTimerFunc(20, &timer, i);
	glutPostRedisplay();
}

void mouse(int x, int y)
{
	//printf("%d %d\n", x, y);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (600, 600);
	glutCreateWindow ("TSBK07 Lab 4");
	glutDisplayFunc(display);
	init ();
	glutTimerFunc(20, &timer, 0);

	glutPassiveMotionFunc(mouse);

	glutMainLoop();
	exit(0);
}

void ballControl(Model* terrain, TextureData* tex, mat4 camMatrix) {

	if (glutKeyIsDown('i')) {
		ballPos.x += moveSpeed;
	}
	if (glutKeyIsDown('j')) {
		ballPos.z -= moveSpeed;
	}
	if (glutKeyIsDown('k')) {
		ballPos.x -= moveSpeed;
	}
	if (glutKeyIsDown('l')) {
		ballPos.z += moveSpeed;
	}
	ballPos.y = getHeight(ballPos.x, ballPos.z, terrain, tex);

	printf("Y pos: %f\n", ballPos.y);
	mat4 ballPosM = T(ballPos.x, ballPos.y, ballPos.z);
	mat4 total = Mult(camMatrix, ballPosM);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
    DrawModel(ballModel, program, "inPosition", "inNormal", "inTexCoord");
}

mat4 keyboardControls() {

    if (glutKeyIsDown('d')) {
    	camDir = MultVec3(Ry(-rotSpeed), camDir);
    }
    if (glutKeyIsDown('a')) {
    	camDir = MultVec3(Ry(rotSpeed), camDir);
    }
	if (glutKeyIsDown('w')) {
		camPos = VectorAdd(camPos, ScalarMult(camDir, moveSpeed));
    }
    if (glutKeyIsDown('s')) {
    	camPos = VectorSub(camPos, ScalarMult(camDir, moveSpeed));
    }
    if (glutKeyIsDown('q')) {
    	camPos = VectorAdd(camPos, ScalarMult(VEC3_UP, moveSpeed));
    }
    if (glutKeyIsDown('e')) {
    	camPos = VectorSub(camPos, ScalarMult(VEC3_UP, moveSpeed));
    }
    
    return lookAtv(camPos, VectorAdd(camPos, camDir), VEC3_UP);
}



// Returns an array containing the neighbour points (stored in the res array)
void getNeighbours(int x, int z, int width, int height, int *res) {

	if (x == width - 1) {
		if (z == 0) {
			res[0] = x;
			res[1] = z;
			res[2] = x-1;
			res[3] = z;
			res[4] = x;
			res[5] = z+1;
		} else if (z == height - 1) {
			res[0] = x;
			res[1] = z-1;
			res[2] = x;
			res[3] = z;
			res[4] = x-1;
			res[5] = z;
		} else {
			res[0] = x;
			res[1] = z-1;
			res[2] = x;
			res[3] = z;
			res[4] = x;
			res[5] = z+1;
		}
	} else {
		if (z == 0) {
			res[0] = x;
			res[1] = z;
			res[2] = x+1;
			res[3] = z;
			res[4] = x;
			res[5] = z+1;	
		} else if (z == height - 1) {
			res[0] = x;
			res[1] = z-1;
			res[2] = x+1;
			res[3] = z;
			res[4] = x;
			res[5] = z;
		} else {
			res[0] = x;
			res[1] = z-1;
			res[2] = x+1;
			res[3] = z;
			res[4] = x;
			res[5] = z+1;
		}
	}
	
}
