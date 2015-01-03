#version 330

in vec3 position;
in vec3 normal;

out vec3 pos;
out vec3 norm;

void main() {
	pos = position;
	norm = normal;
}