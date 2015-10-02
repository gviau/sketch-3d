#version 420

layout(binding = 1) uniform OceanConstants_t
{
    vec4 light_position;
    vec4 ambient_color;
    vec4 diffuse_color;
    vec4 specular_color;

    // x = diffuse, y = specular, z = ambient
    vec4 light_contribution;
};

in vec3 normal;
in vec3 light_dir;
in vec3 eye;

out vec4 color;

float G(float NdotH, float NdotV, float VdotH, float NdotL) {
	float G1 = 2.0 * NdotH * NdotV / VdotH;
	float G2 = 2.0 * NdotH * NdotL / VdotH;
	return min(1.0, min(G1, G2));
}

vec3 R_F(float VdotH) {
	return specular_color.xyz + (1.0 - specular_color) * pow(1.0 - VdotH, 5.0);
}

float Beckmann(float NdotH) {
	const float m = 0.6;
	float A = 1.0 / (pow(m, 2.0) + pow(NdotH, 4.0) * 3.14159265);
	float B = exp( -pow( tan(acos(NdotH)), 2.0) / pow(m, 2.0) );
	return A * B;
}

void main() {
	vec3 N = normalize(normal);
	vec3 V = normalize(eye);
	vec3 L = normalize(light_dir);
	vec3 H = normalize(V + L);

	vec3 specular = vec3(0.0);
	float NdotH = max(0.0, dot(N, H));
	float VdotH = max(0.0, dot(V, H));
	float NdotV = max(0.0, dot(N, V));
	float NdotL = max(0.0, dot(N, L));

	if (NdotL > 0.0 && NdotV > 0.0) {
		specular = (Beckmann(NdotH) * G(NdotH, NdotV, VdotH, NdotL) * R_F(VdotH)) / (NdotL * NdotV);
	}

	color = vec4(pow(NdotL * light_contribution.x * diffuse_color.xyz + light_contribution.y * specular + light_contribution.z * ambient_color.xyz, vec3(1.0 / 2.2)), 1.0);
}