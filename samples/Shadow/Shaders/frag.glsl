#version 330

uniform sampler2D texture0;
uniform sampler2DShadow shadowMap;

uniform vec3 light_position;
uniform vec4 light_color;

in vec3 light_dir;
in vec3 view_dir;
in vec3 normal;
in vec2 uv;
in vec4 shadow_coord;

layout (location=0) out vec4 color;

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine (RenderPassType)
void shadeWithShadow() {
	vec3 L = normalize(light_dir);
	vec3 V = normalize(view_dir);
	vec3 N = normalize(normal);

	float shadow = 0.0;
	vec3 light = vec3(0.0);

	float lambert = max(dot(N, L), 0.0);

	if (lambert > 0.0) {
		light += lambert * light_color.xyz;

		vec3 H = normalize(V + L);
		light += pow(max(dot(H, N), 0.0), light_color.w) * light_color.xyz;
	}

	// PCF
	shadow += textureProjOffset(shadowMap, shadow_coord, ivec2(-1, -1));
	shadow += textureProjOffset(shadowMap, shadow_coord, ivec2(-1,  1));
	shadow += textureProjOffset(shadowMap, shadow_coord, ivec2( 1,  1));
	shadow += textureProjOffset(shadowMap, shadow_coord, ivec2( 1, -1));
	shadow *= 0.25;

	vec3 textureColor = texture(texture0, uv).xyz;
	color = vec4(textureColor * light * shadow + vec3(0.1, 0.1, 0.1), 1.0);
}

subroutine (RenderPassType)
void recordDepth() {
	// Do nothing, depth will be written automatically
}

void main() {
	RenderPass();
}