#version 330

uniform mat4x4 modelViewProjection;
uniform mat4x4 modelView;

layout (location=0) in vec3 in_vertex;
layout (location=1) in vec3 in_normal;

out vec3 position;
out vec3 normal;

void main() {
	position = (modelView * vec4(in_vertex, 1.0)).xyz;
	normal = mat3x3(modelView) * in_normal;
	gl_Position = modelViewProjection * vec4(in_vertex, 1.0);
}