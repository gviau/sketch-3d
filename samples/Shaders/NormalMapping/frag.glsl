#version 330

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

in vec2 uv;
in vec3 light_dir;
in vec3 eye;

out vec4 color;

void main() {
	// The new normal is from the normal map. Since it's stored in tangent space, we can directly use it.
	vec3 N = normalize(texture(normalTexture, uv).xyz);
	vec3 L = normalize(light_dir);
	vec3 V = normalize(eye);

	// Compute diffuse and specular lighting
	vec3 light = vec3(0.0);
	float lambert = max(dot(N, L), 0.0);
	if (lambert > 0.0) {
		vec3 H = normalize(V + L);
		light += lambert * vec3(1.0) +
				 pow(max(dot(H, N), 0.0), 75.0) * vec3(1.0);
	}

	// Gamma correction
	vec3 texColor = pow( texture(diffuseTexture, uv).xyz, vec3(2.2));
	color = vec4( pow( texColor * light, vec3( 1.0 / 2.2)), 1.0);
}