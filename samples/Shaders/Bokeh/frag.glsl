#version 330

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform sampler2D shadowMap;

in vec3 view_dir;
in vec3 normal;
in vec3 tangent;
in vec2 uv;
in vec4 shadow_coord;

in vec3 H0;
in vec3 H1;
in vec3 H2;
in vec3 H3;

out vec4 color;

// From https://mynameismjp.wordpress.com/2011/02/28/bokeh/ code
vec3 GetHBasisIrradiance(vec3 N) {
	vec3 color = vec3(0.0);

	// Band 0
	color += H0 * (1.0 / sqrt(2.0 * 3.14159));

	// Band 1
	color += H1 * -sqrt(1.5 * 3.14159) * N.y;
	color += H2 *  sqrt(1.5 * 3.14159) * (2.0 * N.z - 1.0);
	color += H3 * -sqrt(1.5 / 3.14159) * N.x;

	return color;
}

void main() {
	// Get the normals from the normal map into world space
	vec3 bitangent = cross(normal, tangent);
	mat3 tbn = mat3(tangent, bitangent, normal);

	vec3 L = vec3(0.577, 0.577, 0.577);
	vec3 light_color = vec3(10.0, 8.0, 5.0);
	vec3 N = tbn * normalize(texture(texture1, uv).rgb * 2.0 - 1.0);
	vec3 V = normalize(view_dir);

	// Calculate the visibility factor
	// 4x4 PCF
	vec3 coord = shadow_coord.xyz / shadow_coord.w;
	float shadow = 0.0;
	shadow += (texture(shadowMap, coord.xy).r - 0.001 < coord.z) ? 0.0 : 1.0;
	shadow += (textureOffset(shadowMap, coord.xy, ivec2(0, 1)).r - 0.001 < coord.z) ? 0.0 : 1.0;
	shadow += (textureOffset(shadowMap, coord.xy, ivec2(1, 1)).r - 0.001 < coord.z) ? 0.0 : 1.0;
	shadow += (textureOffset(shadowMap, coord.xy, ivec2(1, 0)).r - 0.001 < coord.z) ? 0.0 : 1.0;
	
	shadow += (textureOffset(shadowMap, coord.xy, ivec2(0,  2)).r - 0.001 < coord.z) ? 0.0 : 1.0;
	shadow += (textureOffset(shadowMap, coord.xy, ivec2(1,  2)).r - 0.001 < coord.z) ? 0.0 : 1.0;
	shadow += (textureOffset(shadowMap, coord.xy, ivec2(2,  2)).r - 0.001 < coord.z) ? 0.0 : 1.0;
	shadow += (textureOffset(shadowMap, coord.xy, ivec2(2,  1)).r - 0.001 < coord.z) ? 0.0 : 1.0;
	shadow += (textureOffset(shadowMap, coord.xy, ivec2(2,  0)).r - 0.001 < coord.z) ? 0.0 : 1.0;
	
	shadow += (textureOffset(shadowMap, coord.xy, ivec2(0,  3)).r - 0.001 < coord.z) ? 0.0 : 1.0;
	shadow += (textureOffset(shadowMap, coord.xy, ivec2(1,  3)).r - 0.001 < coord.z) ? 0.0 : 1.0;
	shadow += (textureOffset(shadowMap, coord.xy, ivec2(2,  3)).r - 0.001 < coord.z) ? 0.0 : 1.0;
	shadow += (textureOffset(shadowMap, coord.xy, ivec2(3,  3)).r - 0.001 < coord.z) ? 0.0 : 1.0;
	shadow += (textureOffset(shadowMap, coord.xy, ivec2(3,  2)).r - 0.001 < coord.z) ? 0.0 : 1.0;
	shadow += (textureOffset(shadowMap, coord.xy, ivec2(3,  1)).r - 0.001 < coord.z) ? 0.0 : 1.0;
	shadow += (textureOffset(shadowMap, coord.xy, ivec2(3,  0)).r - 0.001 < coord.z) ? 0.0 : 1.0;
	
	shadow *= 0.0625;

	vec4 albedo = texture(texture0, uv);
	vec3 diffuse_albedo = pow( albedo.rgb, vec3(2.2) );
	float specular_albedo = albedo.a;

	vec3 bakedLight = GetHBasisIrradiance(N) / 3.14159;
	vec3 diffuse = bakedLight * diffuse_albedo;
	diffuse += light_color / 3.14159 * shadow;

	vec3 H = normalize(V + L);
	float specular_power = 64.0;
	vec3 specular = pow( dot(N, H), specular_power) * (specular_power + 8) / (8 * 3.14159) * light_color * shadow;

	color.rgb = (diffuse * diffuse_albedo + specular * specular_albedo) * max(dot(N, L), 0.0);
	color.a = 1.0;
}