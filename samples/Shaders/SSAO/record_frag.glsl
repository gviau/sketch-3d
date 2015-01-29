#version 330

in vec3 position;
in vec3 normal;

out vec4 pos;
out vec4 norm;

void main() {
	pos = position;
	norm = normal;
}