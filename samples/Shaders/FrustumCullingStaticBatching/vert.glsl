#version 330

uniform mat4 viewProjection;
uniform mat4 view;
uniform mat4 transInvView;

layout (location=0) in vec3 in_vertex;
layout (location=1) in vec3 in_normal;

out vec3 normal;
out vec3 light_dir;

void main() {
	normal = mat3(transInvView) * in_normal;

	vec3 pos = (view * vec4(in_vertex, 1.0)).xyz;
	light_dir = (view * vec4(1.0, 1.0, -3.0, 1.0)).xyz - pos;
	gl_Position = viewProjection * vec4(in_vertex, 1.0);
}