#version 330

uniform mat4 modelViewProjection;
uniform mat4 modelView;
uniform mat3 view;

uniform vec3 light_position;

layout (location=0) in vec3 in_vertex;
layout (location=1) in vec3 in_normal;

out vec3 normal;
out vec3 light_dir;
out vec3 eye;

void main() {
	vec3 pos = (modelView * vec4(in_vertex, 1.0)).xyz;

	normal = normalize(mat3(modelView) * in_normal);
	light_dir = normalize((view * light_position).xyz - pos);
	eye = normalize(-pos);

	gl_Position = modelViewProjection * vec4(in_vertex, 1.0);
}