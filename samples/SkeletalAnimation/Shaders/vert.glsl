#version 330

uniform mat4 modelViewProjection;

layout (location=0) in vec3 in_vertex;
layout (location=1) in vec3 in_normal;
layout (location=2) in vec2 in_uv;

out vec3 normal;
out vec2 uv;

void main() {
	uv = in_uv;
	gl_Position = modelViewProjection * vec4(in_vertex, 1.0);
}