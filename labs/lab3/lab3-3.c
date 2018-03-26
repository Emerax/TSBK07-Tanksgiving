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


#define near 1
#define far 90.0
#define right 1.5
#define left -1.5
#define top 1.5
#define bottom -1.5

GLfloat projectionMatrix[] = {    2.0f*near/(right-left), 0.0f, (right+left)/(right-left), 0.0f,
                                  0.0f, 2.0f*near/(top-bottom), (top+bottom)/(top-bottom), 0.0f,
                                  0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),
                                  0.0f, 0.0f, -1.0f, 0.0f };


GLfloat groundVertices[] = {-100, 0, 100,
					 		-100, 0, -100,
					 		100, 0, 100,
					 		100, 0, -100,
					 		-100, 0, -100,
					 		100, 0, 100};
					 


// Reference to shader program
GLuint program;

// vertex array object
unsigned int vertexArrayObjID;

Model * walls;
Model * blade;
Model * roof;
Model * balcony;
Model * ground;

Model * skybox;

// Texture reference
GLuint myTex;
GLuint skyboxTex;

mat4 trans, rot, windmillTotal;

float dist = 23;

float speed = 0.37;
float rotSpeed = 0.07;
GLfloat controlRot = 0;

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
	program = loadShaders("lab3-3.vert", "lab3-3.frag");
	printError("init shader");
	
	// Upload geometry to the GPU:

	// Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &vertexArrayObjID);
	glBindVertexArray(vertexArrayObjID);
	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &vertexBufferObjID);

/*
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(GLfloat), groundVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));
*/
    // Texture stuff
    glBindTexture(GL_TEXTURE_2D, myTex);
    glUniform1i(glGetUniformLocation(program, "texUnit"), 0); // Texture unit 0

    glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);

    trans = T(0, 0, 0);
    rot = Ry(0);
    windmillTotal = Mult(trans, rot);
	
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
    
    if (glutKeyIsDown('w')) {
    	dist -= speed;
    }
    if (glutKeyIsDown('a')) {
    	controlRot -= rotSpeed;
    }
    if (glutKeyIsDown('s')) {
    	dist += speed;
    }
    if (glutKeyIsDown('d')) {
    	controlRot += rotSpeed;
    }

	mat4 worldToView = lookAt(dist * cos(controlRot), 10, dist * sin(controlRot),
    						  0, 10, 0,
    						  0, 1, 0);


    // SKYBOX WOHOO
    glDisable(GL_DEPTH_TEST); // Disable Z-buffering
    glDisable(GL_CULL_FACE);

	glBindTexture(GL_TEXTURE_2D, skyboxTex);

	mat4 skyboxMat = worldToView;
	skyboxMat.m[3] = 0;
	skyboxMat.m[7] = 0;
	skyboxMat.m[11] = 0;

	mat4 skyboxModelMat = T(0, -0.5, 0);

	glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, skyboxMat.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, skyboxModelMat.m);
    DrawModel(skybox, program, "in_Position", "in_Normal", "inTexCoord");

    glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, worldToView.m);

	glBindTexture(GL_TEXTURE_2D, myTex);



	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, windmillTotal.m);
    DrawModel(walls, program, "in_Position", "in_Normal", "inTexCoord");

    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, windmillTotal.m);
    DrawModel(roof, program, "in_Position", "in_Normal", "inTexCoord");

    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, windmillTotal.m);
    DrawModel(balcony, program, "in_Position", "in_Normal", "inTexCoord");

    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, windmillTotal.m);
    DrawModel(ground, program, "in_Position", "in_Normal", "inTexCoord");

    // WINDMILL BLADES
    mat4 bladeTrans = T(4.5, 9, 0);
    mat4 bladeRot;

    int i;
    for (i = 0; i < 4; ++i) {
    	bladeRot = Rx(t + i * 3.1415 / 2);
    	mat4 bladeMat = Mult(bladeTrans, bladeRot);
    	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, bladeMat.m);
    	DrawModel(blade, program, "in_Position", "in_Normal", "inTexCoord");
    }
	
	printError("display");
	
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow ("GL3 thingey");

    LoadTGATextureSimple("../lab2/maskros512.tga", &myTex);
    LoadTGATextureSimple("SkyBox512.tga", &skyboxTex);

    walls = LoadModelPlus("windmill/windmill-walls.obj");
    blade = LoadModelPlus("windmill/blade.obj");
    roof = LoadModelPlus("windmill/windmill-roof.obj");
    balcony = LoadModelPlus("windmill/windmill-balcony.obj");
    ground = LoadModelPlus("ground_blend.obj");
    skybox = LoadModelPlus("skybox.obj");

	glutDisplayFunc(display); 
	init ();
	OnTimer(20);
	glutMainLoop();
	return 0;
}
