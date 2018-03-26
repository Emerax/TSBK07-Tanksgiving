#version 150

out vec4 out_Color;

in vec3 transformedNormal;
in vec2 texCoord;

uniform sampler2D texUnit;

const vec3 light = vec3(0.58, 0.58, 0.58);

void main(void)
{
	float l = max(dot(-1 * normalize(light), normalize(transformedNormal)), 0);
	out_Color = vec4(transformedNormal, 1.0);
}
