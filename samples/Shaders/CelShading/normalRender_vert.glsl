#version 330

uniform mat4 modelViewProjection;
uniform mat4 modelView;

layout (location = 0) in vec3 in_vertex;
layout (location = 1) in vec3 in_normal;

out vec3 normal;
out vec3 light_dir;

void main() {
	normal = (modelView * vec4(in_normal, 0.0)).xyz;
	vec3 pos = (modelView * vec4(in_vertex, 1.0)).xyz;
	light_dir = vec3(1.0, 1.0, -3.0) - pos;
	gl_Position = modelViewProjection * vec4(in_vertex, 1.0);
}