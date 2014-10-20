#version 330

uniform sampler2D paper;
uniform sampler2D noise;

uniform vec2 thresholds;

in vec2 uv;

out vec4 color;

void main() {
	float noiseValue = length(texture(noise, uv).xyz);
	vec3 fireColor = vec3(1.0);

	if (noiseValue < thresholds.x) {
		discard;
	} else if (noiseValue >= thresholds.x && noiseValue < thresholds.x + thresholds.y) {
		fireColor = vec3(0.35, 0.0, 0.02);
	}

	color = vec4(texture(paper, uv).xyz * fireColor, 1.0);
}