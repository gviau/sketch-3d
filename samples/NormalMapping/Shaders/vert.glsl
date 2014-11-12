#version 330

uniform mat4 modelViewProjection;
uniform mat4 modelView;
uniform mat4 view;

layout (location=0) in vec3 in_vertex;
layout (location=1) in vec3 in_normal;
layout (location=2) in vec2 in_uv;
layout (location=3) in vec3 in_tangent;

out vec2 uv;
out vec3 light_dir;
out vec3 eye;

void main() {
	// Compute the bitangent from the normal and the tangent
	vec3 norm = normalize(mat3(modelView) * in_normal);
	vec3 tangent = normalize(mat3(modelView) * in_tangent);
	vec3 bitangent = normalize(cross(norm, tangent));

	// Construct the TBN matrix which will transform points from the camera space to tangent space
	mat3 tbn = transpose(mat3(
		tangent,
		bitangent,
		norm
	));

	// Transform the light and eye directions from camera space to tangent sapce
	vec3 pos = (modelView * vec4(in_vertex, 1.0)).xyz;
	light_dir = normalize(tbn * ( (view * vec4(-10.0, 3.0, -5.0, 1.0)).xyz - pos));
	eye = tbn * normalize(-pos);

	uv = in_uv;

	gl_Position = modelViewProjection * vec4(in_vertex, 1.0);
}