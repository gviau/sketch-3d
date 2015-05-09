static const int NUM_ITERATIONS = 32;

sampler diffuseTexture;
sampler normalTexture;
sampler heightTexture;

float normalizationFactor;

struct PS_INPUT {
    float4 position : POSITION;
    float3 normal : TEXCOORD0;
    float2 uv : TEXCOORD1;
    float3 light_dir : TEXCOORD2;
    float3 eye : TEXCOORD3;
};

float4 main(PS_INPUT input) : COLOR {
    float3 L = normalize(input.light_dir);
    float3 V = normalize(input.eye);
    float3 N = normalize(input.normal);

    // From http://sunandblackcat.com/tipFullView.php?topicid=28
    float numLayers = lerp(35, 10, abs(dot(float3(0.0, 0.0, 1.0), V)));
    float layerHeight = 1.0 / numLayers;
    float currentLayerHeight = 0.0;
    float2 dtex = 0.1 * V.xy / V.z / numLayers;
    float2 currentTexCoords = input.uv;
    float heightFromTexture = tex2D(heightTexture, currentTexCoords).x;

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        currentLayerHeight += layerHeight;
        currentTexCoords -= dtex;
        heightFromTexture = tex2D(heightTexture, currentTexCoords).x;
    }

    float2 prevTexCoords = currentTexCoords + dtex;

    float nextH = heightFromTexture - currentLayerHeight;
    float prevH = tex2D(heightTexture, prevTexCoords).x - currentLayerHeight + layerHeight;
    float weight = nextH / (nextH - prevH);

    float2 newTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    // Get the new normal from the normal map
    N = normalize(tex2D(normalTexture, newTexCoords).xyz * 2.0 - 1.0);

    // Compute the diffuse and specular colors
    float3 light = float3(0.0, 0.0, 0.0);
    float lambert = max(dot(N, L), 0.0);
    if (lambert > 0.0) {
        float3 H = normalize(V + L);
        light += float3(1.0, 1.0, 1.0) * ( lambert + pow(max(dot(H, N), 0.0), 75.0) );
    }

    // Gamma correction
    float3 texColor = pow( tex2D(diffuseTexture, newTexCoords).xyz, float3(2.2, 2.2, 2.2) );
    return float4( pow( texColor * light, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2) ), 1.0);
}