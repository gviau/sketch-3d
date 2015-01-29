sampler diffuseTexture;
sampler normalTexture;

struct PS_INPUT {
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
    float3 light_dir : TEXCOORD1;
    float3 eye : TEXCOORD2;
};

float4 main(PS_INPUT input) : COLOR {
    float3 N = normalize(tex2D(normalTexture, input.uv).xyz);
    float3 L = normalize(input.light_dir);
    float3 V = normalize(input.eye);

    float3 light = float3(0.0, 0.0, 0.0);
    float lambert = max(dot(N, L), 0.0);
    if (lambert > 0.0) {
        float3 H = normalize(V + L);
        light += lambert * float3(1.0, 1.0, 1.0) +
                 pow(max(dot(H, N), 0.0), 75.0) * float3(1.0, 1.0, 1.0);
    }

    float3 texColor = pow( tex2D(diffuseTexture, input.uv).xyz, float3(2.2, 2.2, 2.2));
    return float4( pow( texColor * light, float3(1.0/2.2, 1.0/2.2, 1.0/2.2)), 1.0);
}