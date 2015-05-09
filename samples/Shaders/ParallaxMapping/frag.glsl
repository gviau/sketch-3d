#version 330

const int NUM_ITERATIONS = 32;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D heightTexture;

uniform float normalizationFactor;

in vec3 normal;
in vec2 uv;
in vec3 light_dir;
in vec3 eye;

out vec4 color;

void main() {
	vec3 L = normalize(light_dir);
	vec3 V = normalize(eye);
	vec3 N = normalize(normal);

	// From http://sunandblackcat.com/tipFullView.php?topicid=28
	float numLayers = mix(35, 10, abs(dot(vec3(0.0, 0.0, 1.0), V)));
	float layerHeight = 1.0 / numLayers;
	float currentLayerHeight = 0.0;
	vec2 dtex = 0.1 * V.xy / V.z / numLayers;
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

	// Get the new normal from the normal map
	N = normalize(texture(normalTexture, newTexCoords).xyz * 2.0 - 1.0);

	// Compute the diffuse and specular colors
	vec3 light = vec3(0.0);
	float lambert = max(dot(N, L), 0.0);
	if (lambert > 0.0) {
		vec3 H = normalize(V + L);
		light += lambert * vec3(1.0) +
				 pow(max(dot(H, N), 0.0), 75.0) * vec3(1.0);
	}

	// Gamma correction
	vec3 texColor = pow( texture(diffuseTexture, newTexCoords).xyz, vec3(2.2) );
	color = vec4( pow( texColor * light, vec3(1.0 / 2.2) ), 1.0);
}