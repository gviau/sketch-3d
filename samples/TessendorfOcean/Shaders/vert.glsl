#version 330

uniform mat4 modelViewProjection;
uniform mat3 modelView;

uniform vec3 light_position;

layout (location=0) in vec3 in_vertex;
layout (location=1) in vec3 in_normal;

out vec3 normal;
out vec3 light_dir;
out vec3 eye;

void main() {
	vec3 pos = modelView * in_vertex;

	normal = modelView * in_normal;
	light_dir = light_position - pos;
	eye = -pos;

	gl_Position = modelViewProjection * vec4(in_vertex, 1.0);
}