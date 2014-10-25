#version 330

uniform mat4 modelViewProjection;
uniform mat3 modelView;

layout (location=0) in vec3 in_vertex;
layout (location=1) in vec3 in_normal;
layout (location=2) in vec2 in_uv;
layout (location=3) in vec3 in_tangent;

out vec2 uv;
out vec3 light_dir;
out vec3 view;

void main() {
	vec3 norm = normalize(modelView * in_normal);
	vec3 tangent = normalize(modelView * in_tangent);
	vec3 bitangent = normalize(cross(norm, tangent));
	mat3 tbn = mat3(
		tangent.x, tangent.y, tangent.z,
		bitangent.x, bitangent.y, bitangent.z,
		norm.x, norm.y, norm.z
	);

	vec3 pos = modelView * in_vertex;
	vec3 ld = normalize((vec3(1.0, 2.0, 6.0) - pos));
	light_dir = tbn * ld;

	vec3 v = normalize(-pos);
	view = tbn * v;

	uv = in_uv;

	gl_Position = modelViewProjection * vec4(in_vertex, 1.0);
}