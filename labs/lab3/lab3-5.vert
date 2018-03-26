#version 150

in vec3 in_Position;

in vec3 in_Normal;

out vec2 texCoord;
in vec2 inTexCoord;

uniform mat4 mdlMatrix;
uniform mat4 camMatrix;

uniform float t;
uniform mat4 myMatrix;
uniform mat4 projectionMatrix;

out vec3 exNormal;
out vec3 exSurface;


void main(void)
{

	exNormal = inverse(transpose(mat3(camMatrix))) * in_Normal;
	exSurface = vec3(camMatrix * mdlMatrix * vec4(in_Position, 1.0));
	gl_Position = projectionMatrix * camMatrix * mdlMatrix * vec4(in_Position, 1.0);

	texCoord = inTexCoord;

}
