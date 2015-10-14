SamplerState samplerState : register(s0);
Texture2D sampleToBlur : register(t0);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
};

static const float weight[4] = { 0.214607, 0.189879, 0.131514, 0.071303 };

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;

    output.color = sampleToBlur.Sample(samplerState, input.uv) * weight[0];
    float step = 1.0 / 384.0;
    
    for (int i = 1; i < 4; i++)
    {
        output.color += sampleToBlur.Sample(samplerState, input.uv + float2(0.0, step * i) ) * weight[i];
        output.color += sampleToBlur.Sample(samplerState, input.uv - float2(0.0, step * i) ) * weight[i];
    }

    return output;
}