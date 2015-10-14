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

float4 suppressLDR(float4 c)
{
    if (c.r > 1.0 || c.g > 1.0 || c.b > 1.0)
    {
        return c;
    }

    return float4(0.0, 0.0, 0.0, 0.0);
}

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;

    float4 texel = fullSample.Sample(samplerState, input.uv);
    output.color = suppressLDR(texel);

    return output;
}