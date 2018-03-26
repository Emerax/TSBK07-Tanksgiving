// Lab 1-1.
// This is the same as the first simple example in the course book,
// but with a few error checks.
// Remember to copy your file to a new on appropriate places during the lab so you keep old results.
// Note that the files "lab1-1.frag", "lab1-1.vert" are required.

// Should work as is on Linux and Mac. MS Windows needs GLEW or glee.
// See separate Visual Studio version of my demos.
#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "loadobj.h"
#include "VectorUtils3.h"

#define near 1.0
#define far 30.0
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5

GLfloat projectionMatrix[] = {    2.0f*near/(right-left), 0.0f, (right+left)/(right-left), 0.0f,
                                  0.0f, 2.0f*near/(top-bottom), (top+bottom)/(top-bottom), 0.0f,
                                  0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),
                                  0.0f, 0.0f, -1.0f, 0.0f };


// Reference to shader program
GLuint program;

// vertex array object
unsigned int vertexArrayObjID;

Model *bunny_m;

Model *teddy_m;

// Texture reference
GLuint myTex;

mat4 trans, rot, bunnyTotal, teddyTotal;

void OnTimer(int value) 
{
    glutPostRedisplay();

    glutTimerFunc(20, &OnTimer, value);
}

void init(void)
{
	// vertex buffer object, used for uploading the geometry
	unsigned int vertexBufferObjID;

	dumpInfo();

	// GL inits
	glClearColor(0.7,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	program = loadShaders("lab2-7.vert", "lab2-7.frag");
	printError("init shader");
	
	// Upload geometry to the GPU:

	// Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &vertexArrayObjID);
	glBindVertexArray(vertexArrayObjID);
	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &vertexBufferObjID);

    // Texture stuff
    glBindTexture(GL_TEXTURE_2D, myTex);
    glUniform1i(glGetUniformLocation(program, "texUnit"), 0); // Texture unit 0

    // glActiveTexture(GL_TEXTURE0); // Select texture unit :D

    glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);

    trans = T(0, 0, 0);
    rot = Ry(0);
    bunnyTotal = Mult(trans, rot);

    trans = T(0, 0, 1);
    rot = Ry(4);
    teddyTotal = Mult(trans, rot);
	
	// End of upload of geometry
	
	printError("init arrays");
}


void display(void)
{
	printError("pre display");

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vertexArrayObjID);	// Select VAO
	glDrawArrays(GL_TRIANGLES, 0, 36 * 3);	// draw object

	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME) / 1000; 
	GLint loc = glGetUniformLocation(program, "t");
	glUniform1f(loc, sin(t));

	GLfloat myMatrix[] = {
		cos(t),      0.0f,     sin(t), 0.0f,
		0.0f,        1.0f,     0.0f,   0.0f,
		-1 * sin(t), 0.0f,    cos(t),   0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	glUniformMatrix4fv(glGetUniformLocation(program, "myMatrix"), 1, GL_TRUE, myMatrix);

	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, bunnyTotal.m);
    DrawModel(bunny_m, program, "in_Position", "in_Normal", "inTexCoord");
	
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, teddyTotal.m);
	DrawModel(teddy_m, program, "in_Position", "in_Normal", "inTexCoord");
	
	GLfloat camX = 5 * sin(t);
	GLfloat camY = 0;
	GLfloat camZ = 5 * cos(t);

	mat4 worldToView = lookAt(camX, camY, camZ,
    						  0, 0, 0,
    						  0, 1, 0);

    glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, worldToView.m);

	
	printError("display");
	
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow ("GL3 thingey");

    bunny_m = LoadModelPlus("bunnyplus.obj");
    teddy_m = LoadModelPlus("cubeplus.obj");

    LoadTGATextureSimple("maskros512.tga", &myTex);

	glutDisplayFunc(display); 
	init ();
	OnTimer(20);
	glutMainLoop();
	return 0;
}
