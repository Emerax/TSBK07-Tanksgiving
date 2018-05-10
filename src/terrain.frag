#version 150

in vec2 texCoord;
in vec3 normal;
in vec3 surface;

out vec4 outColor;

uniform sampler2D tex;

//Hard_coded light source and color, TBD: Replace with in-variables?
vec3 lightSource = vec3(1.0f, 1.0f, 1.0f);
vec3 lightColor = vec3(0.7, 0.7, 0.7);

void main(void) {
	vec3 res = vec3(0, 0, 0);

	// Ambient light component
	float ambient = 0.5;

	//Diffuse light component
	float diffuse;
	diffuse = dot(normalize(normal), lightSource);
	diffuse = max(0.0, diffuse); // No negative light

	//Specular light component
	vec3 r = normalize(reflect(-lightSource, normalize(normal)));
	vec3 v = normalize(-surface);
	float specular = dot(r, v);
	if (specular > 0) {
		specular = 1.0 * pow(specular, 	150);
	}
	specular = max(specular, 0.0);

	float shade = 1 * ambient + 0.7 * diffuse + 0.7 * specular;

	res = shade * lightColor;

	outColor = vec4(res, 1.0f) * texture(tex, texCoord);
	//if(normal.x < 0.1 && normal.y < 0.1 && normal.z < 0.1){
	//	outColor = vec4(1.0f, 0, 0, 1.0f);
	//}
}
