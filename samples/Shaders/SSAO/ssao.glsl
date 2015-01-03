#version 330

uniform sampler2D positions;
uniform sampler2D normals;
uniform sampler2D depthTexture;
uniform sampler2D noise;

uniform vec2 noiseScale;
uniform vec3 kernel[16];
uniform float radius;
uniform mat4 projection;

in vec2 uv;

out vec4 factor;

void main() {
	vec3 pos = texture2D(positions, uv).xyz;
	vec3 N = normalize(texture2D(normals, uv).xyz);
	vec3 V = normalize(-pos);

	// Calculate occlusion factor
	float depth = texture2D(depthTexture, uv).x;
	vec3 origin = V * depth;

	vec3 r = texture2D(noise, uv * noiseScale).xyz * 2.0 - 1.0;
	vec3 tangent = normalize(r - N * dot(r, N));
	vec3 bitangent = cross(N, tangent);
	mat3 tbn = mat3(tangent, bitangent, N);

	float occlusion = 0.0;
	for (int i = 0; i < 16; i++) {
		// Get the sample position
		vec3 samplePos = tbn * kernel[i];
		samplePos = samplePos * radius + origin;

		// Project sample position back in screen space
		vec4 offset = vec4(samplePos, 1.0);
		offset = projection * offset;
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5 + 0.5;

		// Get sample depth
		float sampleDepth = texture2D(depthTexture, offset.xy).r;

		// Range check and accumulate
		float rangeCheck = abs(origin.z - sampleDepth) < radius ? 1.0 : 0.0;
		occlusion += (sampleDepth <= samplePos.z ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 - (occlusion / 16.0);
	factor = vec4(occlusion);
}