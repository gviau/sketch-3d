#version 330

layout (location=0) in vec3 in_vertex;
layout (location=1) in vec3 in_ray;

out vec3 ray;
out vec2 uv;

void main() {
	ray = in_ray;
	uv = in_vertex.xy * 0.5 + 0.5;
	gl_Position = vec4(in_vertex.xy, 0.0, 1.0);
}