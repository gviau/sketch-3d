#version 330

uniform sampler2D texture0;

in vec3 normal;
in vec2 uv;

out vec4 color;

void main() {
	color.rgb = texture(texture0, uv).xyz;
	color.a = 1.0;
}