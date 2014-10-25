#version 330

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

in vec2 uv;
in vec3 light_dir;
in vec3 view;

out vec4 color;

void main() {
	vec3 N = normalize(texture(normalTexture, uv).xyz);
	//N = normalize(normal);
	vec3 L = normalize(light_dir);
	vec3 V = normalize(view);

	vec3 light = vec3(0.0);
	float lambert = max(dot(N, L), 0.0);
	if (lambert > 0.0) {
		vec3 H = normalize(V + L);
		light += lambert * vec3(1.0) +
				 pow(max(dot(H, N), 0.0), 75.0) * vec3(1.0);
	}

	color = vec4(texture(diffuseTexture, uv).xyz * light, 1.0);
	//color = vec4(N, 1.0);
}