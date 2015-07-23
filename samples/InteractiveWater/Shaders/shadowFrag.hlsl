SamplerState textureSamplerState : register(s0);

Texture2D diffuse : register(t0);
Texture2D depthTexture : register(t1);

struct PS_INPUT {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 shadowCoord : TEXCOORD1;
};

float4 main(PS_INPUT input) : SV_TARGET {
    float4 diffuseColor = diffuse.Sample(textureSamplerState, input.uv);

    float3 shadowCoord = input.shadowCoord.xyz / input.shadowCoord.w;
    float depth = depthTexture.Sample(textureSamplerState, shadowCoord.xy).r;

    if (shadowCoord.z < depth) {
        diffuseColor = float4(0.0, 0.0, 0.0, 1.0);
    }

    return diffuseColor;
}