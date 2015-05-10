#version 330

uniform sampler2D sampleToBlur;
//float pixelWeight[8] = { 0.2537, 0.2185, 0.0821, 0.0461, 0.0262, 0.0162, 0.0102, 0.0052 };
const float weight[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 );

in vec2 uv;

out vec4 color;

void main() {
	color = texture(sampleToBlur, uv) * weight[0];
	float step = 1.0 / 384.0;

	for (int i = 1; i < 5; i++) {
		color += texture(sampleToBlur, uv + vec2(0.0, step * i) ) * weight[i];
		color += texture(sampleToBlur, uv - vec2(0.0, step * i) ) * weight[i];
	}
}
