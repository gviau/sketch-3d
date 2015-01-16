#version 330

uniform mat4 modelViewProjection;

uniform float offset;

layout (location = 0) in vec3 in_vertex;
layout (location = 1) in vec3 in_normal;

void main() {
	gl_Position = modelViewProjection * vec4(in_vertex + in_normal * offset, 1.0);
}