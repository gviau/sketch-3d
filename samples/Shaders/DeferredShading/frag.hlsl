sampler positions;
sampler normals;
sampler albedos;

struct PS_INPUT {
    float4 position : POSITION;
    float2 uv : TEXCOORD;
};

float4 main(PS_INPUT input) : COLOR {
    float3 pos = tex2D(positions, input.uv).xyz;
    float3 N = normalize(tex2D(normals, input.uv).xyz);
    float3 albedo = tex2D(albedos, input.uv).xyz;

    float3 light = float3(50, 100, -50);
    float3 L = normalize(light - pos);

    float3 V = normalize(-pos);
    float3 H = normalize(V + L);

    return float4(max(dot(N, L), 0.0) * albedo + pow(max(dot(H, N), 0.0), 100.0) * 1.5, 1.0);
}