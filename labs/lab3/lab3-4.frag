#version 150

out vec4 out_Color;

in vec2 texCoord;

uniform sampler2D texUnit;

vec3 light = vec3(0.58, 0.58, 0.58);

uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent;
uniform bool isDirectional[4];

const vec3 kspec = vec3(1);

in vec3 exNormal;
in vec3 exSurface;

uniform mat4 camMatrix;
uniform mat4 mdlMatrix;
uniform mat4 projectionMatrix;

void main(void)
{

	vec3 res = vec3(0,0,0);

	int i = 0;
	for (i = 0; i < 4; i++) {
		
		if (isDirectional[i]) {
			light = normalize(vec3(camMatrix * vec4(lightSourcesDirPosArr[i], 0.0)));
		} else {
			light = normalize(vec3(camMatrix * vec4(lightSourcesDirPosArr[i], 0.0)) - exSurface);
		}

		// Diffuse
		float diffuse;
		diffuse = dot(normalize(exNormal), light);
		diffuse = max(0.0, diffuse); // No negative light

		// Specular
		vec3 r = reflect(-light, normalize(exNormal));
		vec3 v = normalize(-exSurface);
		float specular = dot(r, v);
		if (specular > 0) {
			specular = 1.0 * pow(specular, specularExponent);
		}

		specular = max(specular, 0);

		float shade = 0.7 * diffuse + 0.3 * specular;

		res = res + shade * lightSourcesColorArr[i];
		
	}

	out_Color = vec4(res, 1.0f) * texture(texUnit, texCoord);

}
