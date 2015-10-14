SamplerState sampler0 : register(s0);

Texture2D paper : register(t0);
Texture2D noise : register(t1);

cbuffer BurningPaperConstants_t : register(b1)
{
    float4 thresholds;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct PS_OUTPUT {
    float4 color : SV_TARGET;
};

PS_OUTPUT main(PS_INPUT input) {
    PS_OUTPUT output;

	// We get the threshold value from the uniform variable and check weither or not the noise texture
	// at this position if less than the threshold. If it is, we discard it. Otherwise, if its value
	// if between a range of threshold, we apply a redish color to simulate burning on the edges
    float noiseValue = length(noise.Sample(sampler0, input.uv).xyz);
    float3 fireColor = float3(1.0, 1.0, 1.0);

    if (noiseValue < thresholds.x) {
        discard;
    } else if (noiseValue >= thresholds.x && noiseValue < thresholds.x + thresholds.y) {
        fireColor = float3(0.55, 0.2, 0.0) * 3.0;
    }

    // Gamma correction
    float3 texColor = pow( paper.Sample(sampler0, input.uv).xyz, float3(2.2, 2.2, 2.2));

    output.color = float4(texColor * fireColor, 1.0);
    
    return output;
}