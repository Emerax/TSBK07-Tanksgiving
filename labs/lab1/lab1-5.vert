#version 150

in  vec3 in_Position;

in vec3 in_Color;
out vec3 frag_Color;

uniform mat4 myMatrix;

void main(void)
{
	gl_Position = myMatrix * vec4(in_Position, 1.0);
	frag_Color = in_Color;

}
