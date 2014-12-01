#version 330

uniform sampler2D positions;
uniform sampler2D normals;
uniform sampler2D albedos;

in vec2 uv;

out vec4 color;

void main() {
	vec3 pos = texture2D(positions, uv).xyz;
	vec3 N = normalize(texture2D(normals, uv).xyz);
	vec3 albedo = texture2D(albedos, uv).xyz;

	vec3 light = vec3(50, 100, -50);
	vec3 L = normalize(light - pos);

	vec3 V = normalize(-pos);
	vec3 H = normalize(V + L);

	color = vec4(max(dot(N, L), 0.0) * albedo + pow(max(dot(H, N), 0.0), 100.0) * 1.5, 1.0);
}