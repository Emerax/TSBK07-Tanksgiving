#version 150

out vec4 outColor;
in vec2 texCoord;
uniform sampler2D tex;
uniform mat4 camMatrix;

in vec3 exNormal;
in vec3 exSurface;
in vec3 pos;

vec3 light = vec3(0, -1, 0);
vec3 lightColor = vec3(1.0, 1.0, 1.0);

void main(void)
{
	vec3 res = vec3(0,0,0);
	//light = normalize(vec3(camMatrix * vec4(light, 0.0)));

	// Diffuse
	float diffuse;
	diffuse = dot(normalize(exNormal), light);
	diffuse = max(0.0, diffuse); // No negative light

	// Specular
	vec3 r = reflect(-light, normalize(exNormal));
	vec3 v = normalize(-exSurface);
	float specular = dot(r, v);
	if (specular > 0) {
		specular = 1.0 * pow(specular, 150);
	}

	specular = max(specular, 0);

	float shade = 0.7 * diffuse + 1 * specular;

	res = shade * lightColor;

	if (pos.y < 0.1) {
		outColor = vec4(0,0,1.0f, 1.0f);
	} else {
		outColor = vec4(res, 1.0f) * texture(tex, texCoord);
	}
	
}
