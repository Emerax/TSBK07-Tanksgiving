#include "falling_snow.h"
#define PI 3.141592
#define MAX_SNOWFLAKES 1000

static const GLfloat snowflake_vertices[] = {
	-0.5f, -0.5f, 0.0f,
 0.5f, -0.5f, 0.0f,
 -0.5f, 0.5f, 0.0f,
 0.5f, 0.5f, 0.0f,
};

static const int LILL_STOFFES_CONSTANT = 4334;
static const float SNOWFLAKE_SPAWN_HEIGHT = 100.0;
static const float SNOWFLAKE_SPAWN_DISTANCE = 15;
static const float SNOWFLAKE_FALL_SPEED = 0.291196; //Chosen by Noora
int i; //Loop variable

GLuint snowflakeProgram;
GLuint snowflakeTexture;

//Shader locations used for calculating billboard rotations.
GLuint camRight;
GLuint camUp;
GLuint billboardMatrix;

//Shader location for the texture.
GLuint sampler;

GLuint snowflakeVertexBuffer;
GLuint snowflakePositionBuffer;

struct snowflake snowflakes[MAX_SNOWFLAKES];

//Every position consists of x, y, z values, requires 3 floats per pos.
static GLfloat snowflakePositions[MAX_SNOWFLAKES * 3];

void initSnowflakes(GLuint program, GLuint texture){
	snowflakeProgram = program;
	snowflakeTexture = texture;

	//Fetching shader locations
	camRight = glGetUniformLocation(snowflakeProgram, "camRight");
	camUp = glGetUniformLocation(snowflakeProgram, "camUp");
	billboardMatrix = glGetUniformLocation(snowflakeProgram, "billboardMatrix");
	sampler = glGetUniformLocation(snowflakeProgram, "tex");

	glGenBuffers(1, &snowflakeVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, snowflakeVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(snowflake_vertices), snowflake_vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &snowflakePositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, snowflakePositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_SNOWFLAKES * 3 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	//Load the texture to be used for snowflakes.
	LoadTGATextureSimple("../assets/Snowflake.tga", &snowflakeTexture);
	//init snowflake structs and array
	for(i = 0; i < MAX_SNOWFLAKES; i++){
		vec2 c = randomUnitCircle(SNOWFLAKE_SPAWN_DISTANCE);
		snowflakes[i] = createSnowflake(c.x, randomFloat(SNOWFLAKE_SPAWN_HEIGHT), c.z, true);
	}
	printf("Snowflakes initialized!\n");
}

void displaySnowflakes(vec3 camPos, mat4 projectionMatrix, mat4 viewMatrix){

	mat4 viewProjectionMatrix = Mult(projectionMatrix, viewMatrix);

	//iterate over snowflakes, update positions
	for(i = 0; i < MAX_SNOWFLAKES; i++){
		struct snowflake s = snowflakes[i];
		if(s.y > 0){
		//Snowflakes above y=0 should continue falling downwards
			s.y -= SNOWFLAKE_FALL_SPEED;
		} else {
		//Snowflakes below y=0 will have their xz-pos randomized and their y set to max.
			vec2 newPos = randomUnitCircle(SNOWFLAKE_SPAWN_DISTANCE);
			s.x = newPos.x;
			s.z = newPos.z;
			s.y = SNOWFLAKE_SPAWN_HEIGHT;
		}
		snowflakes[i] = s;
		//Update the array that will be sent to the GPU
		snowflakePositions[3 * i + 0] = s.x;
		snowflakePositions[3 * i + 1] = s.y;
		snowflakePositions[3 * i + 2] = s.z;

	}

	//Send positions to GPU buffer and bind it.
	glBindBuffer(GL_ARRAY_BUFFER, snowflakePositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_SNOWFLAKES * 3 * sizeof(GLfloat), snowflakePositions, GL_STREAM_DRAW);
	//Activate transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(snowflakeProgram);
	//Bind and activate snowflake texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, snowflakeTexture);
	glUniform1i(sampler, 0);
	//Upload matrices used for calculating billboard rotation
	glUniform3f(camRight, viewMatrix.m[0], viewMatrix.m[1], viewMatrix.m[2]);
	glUniform3f(camUp, viewMatrix.m[4], viewMatrix.m[5], viewMatrix.m[6]);

	glUniformMatrix4fv(billboardMatrix, 1, GL_FALSE, &viewProjectionMatrix.m[0]);

	//Upload vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, snowflakeVertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//Upload positions
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, snowflakePositionBuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//Tell GPU how many buffer values to read for each particle instance
	glVertexAttribDivisor(0, 0); //Move 0 pointers for every vertex, that is, use the same set of vertices for every instance.
	glVertexAttribDivisor(1, 1); //Move 1 pointer for every positions, that is, every instance has a unique position.

	//Finally draw the quads
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, MAX_SNOWFLAKES);
}

struct snowflake createSnowflake(float x, float y, float z, bool active){
	struct snowflake *s = malloc(sizeof(struct snowflake));
	s->x = x;
	s->y = y;
	s->z = z;
	s->active = active;
	return *s;
}

/* Returns a random point inside a circle with radius r */
vec2 randomUnitCircle(float r){
	float a, b, x, z;
	a = randomFloat(r);
	b = randomFloat(r);

	if(b > a){
		x = a * r * cos(2*PI*(b/a));
		z = a * r * sin(2*PI*(b/a));
	} else {
		x = b * r * cos(2*PI*(a/b));
		z = b * r * sin(2*PI*(a/b));
	}
	vec2 res = {x, z};
	//printf("New spawn at x: %f, z: %f\n", x, z);
	return res;
}

float randomFloat(float max){
	return ((float)rand()/(float)(RAND_MAX)) * max;
}

/*
GLuint snowflake_program;
glGenBuffers(1, &billboard_vertex_buffer);
glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
glBufferData(GL_ARRAY_BUFFER, sizeof(snowflake_program), snowflake_program, GL_STATIC_DRAW);

// The VBO containing the positions and sizes of the particles
GLuint particles_position_buffer;
glGenBuffers(1, &particles_position_buffer);
glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
// Initialize with empty (NULL) buffer : it will be updated later, each frame.
glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
*/
