#version 330

uniform sampler2D fullSample;

in vec2 uv;

out vec4 color;

vec4 suppressLDR(vec4 c) {
	if (c.r > 1.0 || c.g > 1.0 || c.b > 1.0) {
		return c;
	}

	return vec4(0.0);
}

void main() {
	vec4 texel = texture(fullSample, uv);
	color = suppressLDR(texel);
}