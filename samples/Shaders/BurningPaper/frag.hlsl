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

float4 main(PS_INPUT input) : SV_TARGET {
	// We get the threshold value from the uniform variable and check weither or not the noise texture
	// at this position if less than the threshold. If it is, we discard it. Otherwise, if its value
	// if between a range of threshold, we apply a redish color to simulate burning on the edges
    float noiseValue = length(noise.Sample(sampler0, input.uv).xyz);
    float3 fireColor = float3(1.0f, 1.0f, 1.0f);

    if (noiseValue < thresholds.x) {
        discard;
    } else if (noiseValue >= thresholds.x && noiseValue < thresholds.x + thresholds.y) {
        fireColor = float3(0.35f, 0.0f, 0.02f);
    }

    // Gamma correction
    float3 texColor = pow( abs(paper.Sample(sampler0, input.uv).xyz), float3(2.2f, 2.2f, 2.2f) );
    float coeff = 1.0f / 2.2f;
    return float4(pow( texColor * fireColor, float3(coeff, coeff, coeff) ), 1.0f);
}