SamplerState sampler0 : register(s0);

Texture2D tex : register(t0);

cbuffer MaterialConstants_t : register(b2) {
    float4 ambientColor;
    float4 diffuseColor;
    float4 specularColorAndPower;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : TEXCOORD1;
};

float4 main(PS_INPUT input) : SV_TARGET {
    return tex.Sample(sampler0, input.uv) * float4(diffuseColor.rgb, 1.0);
}