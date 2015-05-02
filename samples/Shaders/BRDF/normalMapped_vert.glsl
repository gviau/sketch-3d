#version 330

uniform mat4 modelViewProjection;
uniform mat4 modelView;
uniform mat4 view;

uniform vec3 light_position;

layout (location = 0) in vec3 in_vertex;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;
layout (location = 3) in vec3 in_tangent;

out vec3 normal;
out vec2 uv;
out vec3 tangent;
out vec3 light_dir;
out vec3 view_dir;

void main() {
	normal = transpose(inverse(mat3(modelView))) * in_normal;
	uv = in_uv;
	tangent = mat3(modelView) * in_tangent;

	vec3 pos = (modelView * vec4(in_vertex, 1.0)).xyz;
	light_dir = mat3(view) * light_position - pos;
	view_dir = -pos;

	gl_Position = modelViewProjection * vec4(in_vertex, 1.0);
}