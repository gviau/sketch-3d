#version 330

uniform sampler2D texture0;

in vec3 normal;
in vec2 uv;

out vec4 norm;
out vec4 albedo;

void main() {
	norm.xyz = normal;
	albedo.xyz = texture2D(texture0, uv).rgb;
}