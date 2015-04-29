#version 330

uniform mat4 modelViewProjection;
uniform mat4 modelView;
uniform mat4 model;
uniform mat4 shadowMatrix;

uniform sampler2D baked_coeffs;
uniform vec2 baked_size;

layout (location=0) in vec3 in_vertex;
layout (location=1) in vec3 in_normal;
layout (location=2) in vec2 in_uv;
layout (location=3) in vec3 in_tangent;

out vec3 view_dir;
out vec3 normal;
out vec3 tangent;
out vec2 uv;
out vec4 shadow_coord;

out vec3 H0;
out vec3 H1;
out vec3 H2;
out vec3 H3;

void main() {
	normal = normalize(mat3(modelView) * in_normal);
	tangent = normalize(mat3(modelView) * in_tangent);
	
	uv = in_uv;
	gl_Position = modelViewProjection * vec4(in_vertex, 1.0);

	view_dir = -(modelView * vec4(in_vertex, 1.0)).xyz;

	shadow_coord = shadowMatrix * model * vec4(in_vertex, 1.0);

	// Get the baked coefficients
	float y = floor(gl_VertexID / baked_size.x) / baked_size.y;
	float x = (gl_VertexID - y * baked_size.x) / baked_size.x;
	vec4 sample0 = texture(baked_coeffs, vec2(x, y));

	y = floor( (gl_VertexID + 1) / baked_size.x) / baked_size.y;
	x = ( (gl_VertexID + 1) - y * baked_size.x) / baked_size.x;
	vec4 sample1 = texture(baked_coeffs, vec2(x, y));

	y = floor( (gl_VertexID + 2) / baked_size.x) / baked_size.y;
	x = ( (gl_VertexID + 2) - y * baked_size.x) / baked_size.x;
	vec4 sample2 = texture(baked_coeffs, vec2(x, y));

	H0 = vec3(sample0.x, sample1.x, sample2.x);
	H1 = vec3(sample0.y, sample1.y, sample2.y);
	H2 = vec3(sample0.z, sample1.z, sample2.z);
	H3 = vec3(sample0.w, sample1.w, sample2.w);
}