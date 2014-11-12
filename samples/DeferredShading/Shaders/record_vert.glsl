#version 330

uniform mat4 modelViewProjection;
uniform mat4 modelView;

layout (location = 0) in vec3 in_vertex;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

out vec3 normal;
out vec3 pos;
out vec2 uv;

void main() {
	normal = mat3(modelView) * in_normal;
	pos = (modelView * vec4(in_vertex, 1.0)).xyz;
	uv = in_uv;

	gl_Position = modelViewProjection * vec4(in_vertex, 1.0);
}