#version 150

in vec3 inPosition;

out vec3 transformedNormal;
in vec3 inNormal;

out vec2 texCoord;
in vec2 inTexCoord;

uniform mat4 mdlMatrix;
uniform mat4 camMatrix;

uniform float t;
uniform mat4 myMatrix;
uniform mat4 projMatrix;

void main(void)
{
	gl_Position = projMatrix * camMatrix * mdlMatrix * vec4(inPosition, 1.0);

	mat3 normalMatrix1 = mat3(myMatrix);
	transformedNormal = normalMatrix1 * inNormal;

	texCoord = inTexCoord;

}
