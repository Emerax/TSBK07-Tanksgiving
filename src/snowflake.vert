#version 330

layout(location=0) in vec3 vertices;
layout(location=1) in vec3 worldPosition;

uniform mat4 billboardMatrix;
uniform vec3 camRight;
uniform vec3 camUp;

out vec2 texCoord;

void main(void){
	texCoord.s = vertices.x+0.5;
	texCoord.t = vertices.y+0.5;

	vec3 vertexWorldPosition = worldPosition + camRight * vertices.x + camUp * vertices.y;
	gl_Position = billboardMatrix * vec4(vertexWorldPosition, 1.0f);
}
