#version 330

uniform sampler2D positions;
uniform sampler2D normals;
uniform sampler2D ssao;
uniform int doSsao;

in vec2 uv;

out vec4 color;

void main() {
	vec3 pos = texture2D(positions, uv).xyz;
	vec3 N = normalize(texture2D(normals, uv).xyz);
	vec3 V = normalize(-pos);
	vec3 L = normalize(vec3(-10.0, 3.0, -5.0) - pos);
	vec3 H = normalize(V + L);
	color.rgb = vec3(0.1, 0.1, 0.1);

	float lambert = max(dot(N, L), 0.0);
	if (lambert > 0.0) {
		color.rgb += vec3(1.0, 1.0, 1.0) * lambert;
		color.rgb += vec3(1.0, 1.0, 1.0) * pow(max(dot(N, H), 0.0), 75.0);
	}

	if (doSsao != 0) {
		// First, blur the ssao occlusion factor to remove the 4x4 noise pattern
		vec2 texelSize = 1.0 / vec2(textureSize(ssao, 0));
		float result = 0.0;
		vec2 hlim = vec2(-4.0 * 0.5 + 0.5);
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				vec2 offset = (hlim + vec2(float(i), float(j))) * texelSize;
				result += texture2D(ssao, uv + offset).r;
			}
		}
		color.rgb = color.rgb * (result / 16.0);
	}

	color.a = 1.0;
}