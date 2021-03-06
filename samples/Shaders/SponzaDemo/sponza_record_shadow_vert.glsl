#version 330

uniform mat4 modelViewProjection;

layout (location=0) in vec3 in_vertex;

out vec4 position;

void main() {
	gl_Position = modelViewProjection * vec4(in_vertex, 1.0);
	position = gl_Position;
}