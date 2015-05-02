#version 330

uniform float specular_power;
uniform int brdfIndex;

uniform float roughness;
uniform float R0;

in vec3 normal;
in vec2 uv;
in vec3 light_dir;
in vec3 view_dir;

out vec4 color;

vec3 textbookBlinnPhong(vec3 N, vec3 L, vec3 H, vec3 diffuse_color, vec3 specular_color);
vec3 blinnPhong(vec3 N, vec3 L, vec3 H, vec3 diffuse_color, vec3 specular_color);
vec3 cookTorrance(vec3 N, vec3 L, vec3 H, vec3 V, vec3 diffuse_color, vec3 specular_color);

void main() {
	const vec3 diffuse_color = vec3(1.0, 0.01, 0.01);
	const vec3 specular_color = vec3(1.0, 1.0, 1.0);

	vec3 N = normalize(normal);
	vec3 L = normalize(light_dir);
	vec3 V = normalize(view_dir);
	vec3 H = normalize(V + L);

	if (brdfIndex == 0) {
		color.rgb = pow( textbookBlinnPhong(N, L, H, diffuse_color, specular_color), vec3(1.0 / 2.2) );
	} else if (brdfIndex == 1) {
		color.rgb = pow( blinnPhong(N, L, H, diffuse_color, specular_color), vec3(1.0 / 2.2) );
	} else if (brdfIndex == 2) {
		color.rgb = cookTorrance(N, L, H, V, diffuse_color * 5.0, specular_color * 5.0);
	}

	color.a = 1.0;
}

vec3 textbookBlinnPhong(vec3 N, vec3 L, vec3 H, vec3 diffuse_color, vec3 specular_color) {
	float lambert = max(dot(N, L), 0.0);
	vec3 diffuse = lambert * diffuse_color;

	vec3 specular = pow( max(dot(H, N), 0.0), specular_power) * specular_color;

	return diffuse + specular;
}

vec3 blinnPhong(vec3 N, vec3 L, vec3 H, vec3 diffuse_color, vec3 specular_color) {
	vec3 diffuse = diffuse_color / 3.14159;
	vec3 specular = pow( max(dot(H, N), 0.0), specular_power) * ((8 + specular_power) / 25.13272) * specular_color;

	return (diffuse + specular) * max(dot(N, L), 0.0);
}

vec3 cookTorrance(vec3 N, vec3 L, vec3 H, vec3 V, vec3 diffuse_color, vec3 specular_color) {
	// BRDF's parts from http://graphicrants.blogspot.fr/2013/08/specular-brdf-reference.html
	float nDotL = clamp(dot(N, L), 0.0, 1.0);
	float nDotH = clamp(dot(N, H), 0.0, 1.0);
	float nDotV = clamp(dot(N, V), 0.0, 1.0);
	float vDotH = clamp(dot(V, H), 0.0, 1.0);
	float roughness_sq = roughness * roughness;

	// Beckmann distribution
	float nDotH_2 = nDotH * nDotH;
	float D = exp( (nDotH_2 - 1.0) / (roughness_sq * nDotH_2) ) / (3.14159 * roughness_sq * nDotH_2 * nDotH_2);

	// Fresnel term using Schlick's approximation
	float F = R0 + (1.0 - R0) * pow(1.0 - vDotH, 5.0);

	// Geometric term
	float G1 = 2.0 * nDotH * nDotV / vDotH;
	float G2 = 2.0 * nDotH * nDotL / vDotH;
	float G = min(1.0, min(G1, G2));

	float kSpec = D * F * G / (4.0 * nDotV * nDotL);

	vec3 diffuse = diffuse_color / 3.14159;
	vec3 specular = kSpec * specular_color;

	return (diffuse + specular) * nDotL;
}