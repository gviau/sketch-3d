#version 330

uniform sampler2D paper;
uniform sampler2D noise;

uniform vec2 thresholds;

in vec2 uv;

out vec4 color;

void main() {
	// We get the threshold value from the uniform variable and check weither or not the noise texture
	// at this position if less than the threshold. If it is, we discard it. Otherwise, if its value
	// if between a range of threshold, we apply a redish color to simulate burning on the edges
	float noiseValue = length(texture(noise, uv).xyz);
	vec3 fireColor = vec3(1.0);

	if (noiseValue < thresholds.x) {
		discard;
	} else if (noiseValue >= thresholds.x && noiseValue < thresholds.x + thresholds.y) {
		fireColor = vec3(0.35, 0.0, 0.02);
	}

	// Gamma correction
	vec3 texColor = pow( texture(paper, uv).xyz, vec3(2.2) );
	color = vec4(pow( texColor * fireColor, vec3(1.0 / 2.2) ), 1.0);
}