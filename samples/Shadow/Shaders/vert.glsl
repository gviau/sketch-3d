#version 330

uniform mat4 modelViewProjection;
uniform mat3 modelView;
uniform mat4 model;
uniform mat4 shadowMatrix;

uniform vec3 light_position;

layout (location=0) in vec3 in_vertex;
layout (location=1) in vec3 in_normal;
layout (location=2) in vec2 in_uv;

out vec3 light_dir;
out vec3 view_dir;
out vec3 normal;
out vec2 uv;
out vec4 shadow_coord;

void main() {
	vec3 pos = modelView * in_vertex;
	light_dir = light_position - pos;
	view_dir = -pos;
	normal = modelView * in_normal;

	uv = in_uv;
	gl_Position = modelViewProjection * vec4(in_vertex, 1.0);

	// shadowMatrix converts from model coordinates to shadow map coordinates
	shadow_coord = shadowMatrix * model * vec4(in_vertex, 1.0);
}