#version 330

uniform sampler2D debug;

in vec2 uv;

out vec4 color;

void main() {
	color.rgb = texture(debug, uv).rgb;
	color.a = 1.0;
}