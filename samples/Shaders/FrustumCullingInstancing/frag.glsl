#version 330

in vec3 normal;
in vec3 light_dir;

out vec4 color;

void main() {
	vec3 N = normalize(normal);
	vec3 L = normalize(light_dir);
	
	color.rgb = vec3(0.0);
	color.a = 1.0;

	float lambert = max(dot(N, L), 0.0);
	if (lambert > 0.0) {
		color.rgb = lambert * vec3(1.0);
	}
}