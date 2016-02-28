struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : TEXCOORD0;
    float3 lightDir : TEXCOORD1;
    float3 eyeDir : TEXCOORD2;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
};

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;

    float3 N = normalize(input.normal);
    float3 L = normalize(input.lightDir);
    float3 V = normalize(input.eyeDir);

    float NdotL = max(dot(N, L), 0.0);

    output.color = float4(N, 1.0);

    return output;
}