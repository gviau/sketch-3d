#version 330

uniform mat4 modelViewProjection;

layout (location = 0) in vec3 in_vertex;

out vec2 uv;

void main() {
	uv = in_vertex.xy * 0.5 + 0.5;
	gl_Position = vec4(in_vertex.xy, 0.0, 1.0);
	//gl_Position = modelViewProjection * vec4(in_vertex, 1.0);
}