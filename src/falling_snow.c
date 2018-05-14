#include "falling_snow.h"
#define PI 3.141592
//One hundred thousand.
#define MAX_SNOWFLAKES 100000

static const GLfloat snowflake_vertices[] = {
	-0.5f, -0.5f, 0.0f,
 0.5f, -0.5f, 0.0f,
 -0.5f, 0.5f, 0.0f,
 0.5f, 0.5f, 0.0f,
};

static const vec3 UP = {0, 1, 0};
static const vec3 RIGHT = {1, 0, 0};

static const int INITIAL_SNOWFLAKES = 1000;
static const float SNOWFLAKE_SPAWN_HEIGHT = 50.0;
static const float SNOWFLAKE_SPAWN_DISTANCE = 30;
static const float SNOWFLAKE_TERMINAL_V = -0.291196; //Chosen by Noora.
static const float SNOWFLAKE_DELTA_V = 0.02; //Very gravity.
static const float VELOCITY_MODIFIER = 0.99999; //For horizontal velocity.
//Standard velocity vector causing snowflakes to fall downwards
int i; //Loop variable
int snowflakeCounter = 0; //Keeps track of last-activated snowflake in their list.

static const int SNOWFLAKES_PER_TICK = 20; //Numer to spawn per tick


GLuint snowflakeProgram;
GLuint snowflakeTexture;

//Shader locations used for calculating billboard rotations.
GLuint camRight;
GLuint camUp;
GLuint billboardMatrix; //This is the view-projection matrix.

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
		snowflakes[i] = createSnowflake(c.x, randomFloat(SNOWFLAKE_SPAWN_HEIGHT), c.z, false);
		if(i < INITIAL_SNOWFLAKES){
			snowflakes[i].active = true;
		}
	}
	printf("Snowflakes initialized!\n");
}

void displaySnowflakes(vec3 camPos, mat4 projectionMatrix, mat4 viewMatrix){

	mat4 viewProjectionMatrix = Mult(projectionMatrix, viewMatrix);

	/*Activate some new snowflakes
		Note that this will loop forever if no inactive snowflakes are found.
		Not a problem so long as MAX_SNOWFLAKES is large enough.
	*/
	int n = SNOWFLAKES_PER_TICK;
	while(n > 0){
		int si = nextSnowflake();
		struct snowflake s = snowflakes[si];
			//Give the snowflake new coordinates and activate it
			vec2 newPos = randomUnitCircle(SNOWFLAKE_SPAWN_DISTANCE);
			s.x = camPos.x + newPos.x;
			s.z = camPos.z + newPos.z;
			s.y = SNOWFLAKE_SPAWN_HEIGHT;
			s.active = true;
			//Normal snowflakes should just fall down at a set speed.
			s.vx = 0;
			s.vy = SNOWFLAKE_TERMINAL_V;
			s.vz = 0;
			//A snowflake was successfully allocated!
			--n;
			snowflakes[si] = s;
	}

	//iterate over snowflakes, update position
	for(i = 0; i < MAX_SNOWFLAKES; i++){
		struct snowflake s = snowflakes[i];
		//Only update active snowflakes
		if(s.active){
			if(s.y > 0){
				//Snowflakes above y=0 should continue simulating movement
				s.x += s.vx;
				s.y += s.vy;
				s.z += s.vz;
				//X-, Z-velocities approach 0 as time goes on, lazy simulation of ballistic trajectories.
				s.vx *= VELOCITY_MODIFIER;
				s.vz *= VELOCITY_MODIFIER;
				if(s.vy >= SNOWFLAKE_TERMINAL_V){
					s.vy -= SNOWFLAKE_DELTA_V;
				}
			} else {
				//Snowflakes below y=0 will be disabled and have their y set to max.
				s.y = SNOWFLAKE_SPAWN_HEIGHT;
				s.active = false;
			}
			snowflakes[i] = s;
			//Update the array that will be sent to the GPU
			snowflakePositions[3 * i + 0] = s.x;
			snowflakePositions[3 * i + 1] = s.y;
			snowflakePositions[3 * i + 2] = s.z;
		}
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
	//viewMatrix orthogonal, traspose is inverse.
	mat4 inverseCam = Transpose(viewMatrix);
	//camRight and camUp given by moving (1, 0, 0) and (0, 1, 0) into worldspace
	vec3 r = MultVec3(inverseCam, RIGHT);
	vec3 u = MultVec3(inverseCam, UP);
	glUniform3f(camRight, r.x, r.y, r.z);
	glUniform3f(camUp, u.x, u.y, u.z);

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

/* Spawns a snosplosion consisting of density particles at pos.
	They will all explode away from pos in random directions, increase the force
	variable to increase initial speed. */
void spawnSnosplosion(vec3 pos, int density, float force){
	for(i = 0; i <= density; ++i){
		int si = nextSnowflake();
		struct snowflake s = snowflakes[si];
		//Set snowflakes position to the explosion point.
		s.x = pos.x;
		s.y = pos.y;
		s.z = pos.z;
		//Generate a unit-sphere location to act as velocity for this particle.
		vec2 dirxz = randomUnitCircle(force);

		s.vx = dirxz.x;
		s.vz = dirxz.z;
		s.vy = randomUnitCircle(force).x; //Ugly hack

		//Finally activate the snowflake
		s.active = true;
		snowflakes[si] = s;
	}
}

//Returns the index of the next available snowflake (i.e. non-active one)
int nextSnowflake(){
	int j;
	//Start searching at the last pos of snowflakeCounter, means this function usually returns immediately.
	for(j = snowflakeCounter; j < MAX_SNOWFLAKES; ++j){
		if(!snowflakes[j].active){
			//Found one!
			snowflakeCounter = j + 1;
			if(snowflakeCounter >= MAX_SNOWFLAKES){
				//End of list eached, wrap around.
				snowflakeCounter = 0;
			}
			return j;
		}
	}
	//End of list reached, start searching from the beginning.
	for(j = 0; j < snowflakeCounter; ++j){
		if(!snowflakes[j].active){
			//Found one!
			snowflakeCounter = j + 1;
			if(snowflakeCounter >= MAX_SNOWFLAKES){
				//End of list eached, wrap around.
				snowflakeCounter = 0;
			}
			return j;
		}
	}
	//No non-active index found, return 0 and hope for the best.
	return 0;
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
