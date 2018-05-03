#version 150

// TANK.VERT //

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;

out vec2 texCoord;
out vec3 normal;
out vec3 surface;

// NY
uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform mat4 camMatrix;

void main(void) {
	texCoord = inTexCoord;
	gl_Position = projMatrix * camMatrix * mdlMatrix * vec4(inPosition, 1.0);
	normal = vec3(mdlMatrix * vec4(inNormal, 0.0));
	//normal = inNormal;
	surface = vec3(camMatrix * mdlMatrix * vec4(inPosition, 1.0));
}
