#version 150

in  vec3 in_Position;

out vec3 normal;
in vec3 in_Normal;

out vec2 texCoord;
in vec2 inTexCoord;

uniform float t;

uniform mat4 myMatrix;

void main(void)
{
	gl_Position = myMatrix * vec4(in_Position, 1.0);
	normal = in_Normal;
	texCoord = inTexCoord;

}
