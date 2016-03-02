SamplerState samplerState : register(s0);
Texture2D texture1 : register(t0);
Texture2D texture2 : register(t1);

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

    float2 uv = input.uv;
    uv.y = 1.0 - uv.y;

    output.color = texture1.Sample(samplerState, uv) * texture2.Sample(samplerState, input.uv);

    return output;
}