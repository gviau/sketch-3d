#version 330

uniform mat4 modelViewProjection;

layout (location=0) in vec3 in_vertex;
layout (location=1) in vec2 in_uv;

out vec2 uv;

void main() {
	// Pass through shader
	uv = in_uv;
	gl_Position = modelViewProjection * vec4(in_vertex, 1.0);
}