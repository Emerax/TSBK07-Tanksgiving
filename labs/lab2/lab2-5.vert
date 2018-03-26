#version 150

in  vec3 in_Position;

out vec3 transformedNormal;
in vec3 in_Normal;

out vec2 texCoord;
in vec2 inTexCoord;

uniform mat4 mdlMatrix;
uniform mat4 camMatrix;

uniform float t;
uniform mat4 myMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
	gl_Position = projectionMatrix * camMatrix * mdlMatrix * vec4(in_Position, 1.0);

	mat3 normalMatrix1 = mat3(myMatrix);
	transformedNormal = normalMatrix1 * in_Normal;

	texCoord = inTexCoord;

}
