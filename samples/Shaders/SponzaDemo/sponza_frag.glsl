#version 330

uniform sampler2D texture0;

in vec3 normal;
in vec2 uv;

out vec3 norm;
out vec3 albedo;

void main() {
	norm = normal;
	albedo = texture2D(texture0, uv).rgb;
}