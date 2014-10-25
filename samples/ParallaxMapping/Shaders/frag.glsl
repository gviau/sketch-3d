#version 330

const int NUM_ITERATIONS = 32;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D heightTexture;

uniform float normalizationFactor;

in vec3 normal;
in vec2 uv;
in vec3 light_dir;
in vec3 view;

out vec4 color;

void main() {
	vec3 L = normalize(light_dir);
	vec3 V = normalize(view);
	vec3 N = normalize(normal);

	float numLayers = mix(15, 10, abs(dot(N, V)));
	float layerHeight = 1.0 / numLayers;
	float currentLayerHeight = 0.0;
	vec2 dtex = 0.1 * V.xy / numLayers;
	vec2 currentTexCoords = uv;
	float heightFromTexture = texture(heightTexture, currentTexCoords).x;

	while (heightFromTexture > currentLayerHeight) {
		currentLayerHeight += layerHeight;
		currentTexCoords -= dtex;
		heightFromTexture = texture(heightTexture, currentTexCoords).x;
	}

	vec2 prevTexCoords = currentTexCoords + dtex;

	float nextH = heightFromTexture - currentLayerHeight;
	float prevH = texture(heightTexture, prevTexCoords).x - currentLayerHeight + layerHeight;
	float weight = nextH / (nextH - prevH);

	vec2 newTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	N = normalize(texture(normalTexture, newTexCoords).xyz);
	//N = normalize(normal);

	vec3 light = vec3(0.0);
	float lambert = max(dot(N, L), 0.0);
	if (lambert > 0.0) {
		vec3 H = normalize(V + L);
		light += lambert * vec3(1.0) +
				 pow(max(dot(H, N), 0.0), 75.0) * vec3(1.0);
	}

	color = vec4(texture(diffuseTexture, newTexCoords).xyz * light, 1.0);
}