#version 150

out vec4 out_Color;

in vec3 normal;

in vec2 texCoord;

void main(void)
{
	out_Color = vec4(texCoord, 1.0, 1.0);
}
