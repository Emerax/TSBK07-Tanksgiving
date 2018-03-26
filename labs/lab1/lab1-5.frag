#version 150

out vec4 out_Color;

in vec3 frag_Color;

void main(void)
{
	out_Color = vec4(frag_Color, 1.0);
}
