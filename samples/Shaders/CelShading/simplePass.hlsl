SamplerState samplerState : register(s0);
Texture2D fullSample : register(t0);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
};

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    output.color = float4(1.0, 1.0, 1.0, 1.0);

    return output;
}