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

PS_OUTPUT main(PS_INPUT input) {
    PS_OUTPUT output;

    float3 N = normalize(input.normal);
    float3 L = normalize(input.lightDir);
    float3 V = normalize(input.eyeDir);

    float NdotL = max(dot(N, L), 0.0);

	if (NdotL > 0.95)
    {
		output.color = float4(1.0, 0.5, 0.5, 1.0);
    }
	else if (NdotL > 0.5)
    {
		output.color = float4(0.6, 0.3, 0.3, 1.0);
    }
	else if (NdotL > 0.25)
    {
		output.color = float4(0.4, 0.2, 0.2, 1.0);
    }
	else
    {
		output.color = float4(0.2, 0.1, 0.1, 1.0);
    }

    return output;;
}