#version 330

uniform sampler2D texture0;	// Diffuse texture
uniform sampler2D texture1;	// Normal map
uniform sampler2D texture2; // Specular map

uniform int var;

in vec3 normal;
in vec2 uv;
in vec3 tangent;

out vec3 norm;
out vec3 albedo;

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

// Record depth only
subroutine (RenderPassType)
void record_depth() {
	// Do nothing, depth will be written automatically
}

// Record the geometry normally
subroutine (RenderPassType)
void record_geometry() {
	vec3 N = normalize(normal);
	vec3 T = normalize(tangent);
	vec3 B = cross(N, T);
	mat3 tangentToWorld = mat3(T, B, N);

	norm = tangentToWorld * (texture2D(texture1, uv).rgb * 2.0 - 1.0);
	albedo = texture2D(texture0, uv).rgb;
}

void main() {
	RenderPass();
}