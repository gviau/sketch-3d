#version 330

uniform sampler2D texture0;

in vec3 normal;
in vec2 uv;

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
	norm = normal;
	albedo = texture2D(texture0, uv).rgb;
}

void main() {
	RenderPass();
}