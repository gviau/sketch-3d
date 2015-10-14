SamplerState samplerState : register(s0);
Texture2D HDRTexture : register(t0);
Texture2D bluredHDR : register(t1);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
};

// From http://frictionalgames.blogspot.ca/2012/09/tech-feature-hdr-lightning.html
float3 Uncharted2Tonemap(float3 x) {
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((x * (A*x + C*B) + D*E) / (x * (A*x + B) + D*F)) - E/F;
}

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;

    float2 blurUv = input.uv;
    blurUv.y = 1.0 - blurUv.y;

    float3 hdr = HDRTexture.Sample(samplerState, input.uv).rgb;
    float3 blur = bluredHDR.Sample(samplerState, blurUv).rgb;
    float3 composedColor = hdr + blur;

    float coeff = 1.0 / 2.2;
    output.color.rgb = pow( composedColor, float3(coeff, coeff, coeff) );
    output.color.a = 1.0;

    return output;
};