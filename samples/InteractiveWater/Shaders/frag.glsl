#version 330

uniform sampler2D waterTexture;

in vec3 normal;
in vec2 uv;
in vec3 light_dir;
in vec3 view_dir;

out vec4 color;

void main() {
	vec3 N = normalize(normal);
	vec3 L = normalize(light_dir);
	vec3 V = normalize(view_dir);
	vec3 H = normalize(V + L);

	color = vec4( max(dot(N, L), 0.0) * vec3(0.5, 0.65, 0.75) + pow(max(dot(H, N), 0.0), 35.0) + vec3(0.1, 0.1, 0.1), 1.0);
	//color = vec4(texture2D(waterTexture, uv).xyz, 1.0);
}