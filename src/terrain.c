#include "terrain.h"

vec3 genV(int x, int z, GLfloat *vertexArray, TextureData *tex) {
	vec3 res;
	res.x = vertexArray[(x + z * tex->width)*3 + 0];
	res.y = vertexArray[(x + z * tex->width)*3 + 1];
	res.z = vertexArray[(x + z * tex->width)*3 + 2];

	return res;
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
			getNeighbours(x, z, tex->width, tex->height, n);

			vec3 normal = CrossProduct(VectorSub(genV(n[4], n[5], vertexArray, tex), genV(n[0], n[1], vertexArray, tex)),
										VectorSub(genV(n[2], n[3], vertexArray, tex), genV(n[0], n[1], vertexArray, tex)));

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

float getHeight(float x, float z, Model* terrain, TextureData* tex) {

	int floorX = floor(x);
	int floorZ = floor(z);

	if(floorX < 0 || floorX >= tex->width || floorZ < 0 || floorZ >= tex->height){
		return 0;
	}

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
