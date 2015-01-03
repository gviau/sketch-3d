#version 330

uniform mat4 modelView;
uniform mat4 view;

uniform sampler2D normals;
uniform sampler2D albedos;
uniform sampler2D depth;
uniform sampler2DShadow shadowMap0;

uniform vec2 positionReconstructionParams;

uniform vec3 ambientLight;

uniform vec3 lightPosition0;
uniform vec3 lightColor0;
uniform vec2 lightRadius_Power0;
uniform mat4 shadowMatrix0;

uniform vec3 lightPosition1;
uniform vec3 lightColor1;
uniform vec2 lightRadius_Power1;

uniform vec3 lightPosition2;
uniform vec3 lightColor2;
uniform vec2 lightRadius_Power2;

in vec3 ray;
in vec2 uv;

out vec4 color;

void main() {
	// Reconstruct position from depth texture:
	//	1. Transform the normalized device coordinate depth value into the view value (range [-1, 1])
	//	2. Solve for zView using values from the projection matrix
	//	3. Use the precomputed interpolated ray from each corner of the quad;
	float zNdc = 2.0 * texture2D(depth, uv).r - 1.0;
	float zView = -positionReconstructionParams.x / (zNdc + positionReconstructionParams.y);
	vec3 pos = zView * ray;

	vec3 worldPos = (inverse(view) * vec4(pos, 1.0)).xyz;

	vec3 V = normalize(-pos);
	vec3 N = normalize(texture2D(normals, uv).xyz);
	color.rgb = ambientLight;

	///////////////////////////////////////////////////////////////////////////
	// Light #0
	vec4 shadowCoord0 = shadowMatrix0 * vec4(worldPos, 1.0);
	float shadow = 0.0;
	shadow += textureProjOffset(shadowMap0, shadowCoord0, ivec2(-1, -1));
	shadow += textureProjOffset(shadowMap0, shadowCoord0, ivec2(-1,  1));
	shadow += textureProjOffset(shadowMap0, shadowCoord0, ivec2( 1,  1));
	shadow += textureProjOffset(shadowMap0, shadowCoord0, ivec2( 1, -1));
	shadow *= 0.25;

	vec3 L0 = normalize( (modelView * vec4(lightPosition0, 1.0)).xyz - pos);
	float lambert0 = max(dot(N, L0), 0.0);
	color.rgb += lambert0 * lightColor0 * shadow;

	/*
	///////////////////////////////////////////////////////////////////////////
	// Light #1
	vec3 L1 = normalize( (modelView * vec4(lightPosition1, 1.0)).xyz - pos);
	float lambert1 = max(dot(N, L1), 0.0);
	color.rgb += lambert1 * lightColor0;

	///////////////////////////////////////////////////////////////////////////
	// Light #2
	vec3 L2 = normalize( (modelView * vec4(lightPosition2, 1.0)).xyz - pos);
	float lambert2 = max(dot(N, L2), 0.0);
	color.rgb += lambert2 * lightColor0;
	*/
	vec3 albedo = texture2D(albedos, uv).rgb;

	// Gamma correction
	color.rgb = pow(color.rgb, vec3(1.0 / 2.2)) * albedo;
	color.a = 1.0;
}