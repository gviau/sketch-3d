#version 330

in vec3 normal;
in vec3 light_dir;
in vec3 view_dir;

out vec4 color;

void main() {
	vec3 N = normalize(normal);
	vec3 L = normalize(light_dir);
	vec3 V = normalize(view_dir);

	float lambert = max(dot(N, L), 0.0);
	/*float edge = max(dot(N, V), 0.0);
	if (edge < 0.25) {
		edge = 0.0;
	}*/

	color.rgb = (lambert > 0.95 ? vec3(1.0, 0.5, 0.5) : lambert > 0.5 ? vec3(0.6, 0.3, 0.3) : lambert > 0.25 ? vec3(0.4, 0.2, 0.2) : vec3(0.2, 0.1, 0.1));
	//color.rgb *= edge;
	color.a = 1.0;
}