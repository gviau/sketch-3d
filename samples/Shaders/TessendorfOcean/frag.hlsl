cbuffer OceanConstants_t : register(b1)
{
    float4 light_position;
    float4 ambient_color;
    float4 diffuse_color;
    float4 specular_color;
    float4 light_contribution;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : TEXCOORD0;
    float3 light_dir : TEXCOORD1;
    float3 eye : TEXCOORD2;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
};

float G(float NdotH, float NdotV, float VdotH, float NdotL)
{
    float G1 = 2.0 * NdotH * NdotV / VdotH;
    float G2 = 2.0 * NdotH * NdotL / VdotH;
    return min(1.0, min(G1, G2));
}

float3 R_F(float VdotH)
{
    return specular_color.xyz + (1.0 - specular_color.xyz) * pow(1.0 - VdotH, 5.0);
}

float Beckmann(float NdotH) {
	const float m = 0.6;
	float A = 1.0 / (pow(m, 2.0) + pow(NdotH, 4.0) * 3.14159265);
	float B = exp( -pow( tan(acos(NdotH)), 2.0) / pow(m, 2.0) );
	return A * B;
}

PS_OUTPUT main(PS_INPUT input) {
    PS_OUTPUT output;
    
	float3 N = normalize(input.normal);
	float3 V = normalize(input.eye);
	float3 L = normalize(input.light_dir);
	float3 H = normalize(V + L);

	float3 specular = float3(0.0, 0.0, 0.0);
	float NdotH = max(0.0, dot(N, H));
	float VdotH = max(0.0, dot(V, H));
	float NdotV = max(0.0, dot(N, V));
	float NdotL = max(0.0, dot(N, L));

	if (NdotL > 0.0 && NdotV > 0.0) {
		specular = (Beckmann(NdotH) * G(NdotH, NdotV, VdotH, NdotL) * R_F(VdotH)) / (NdotL * NdotV);
	}

    float coeff = 1.0 / 2.2;
	output.color = float4(pow(NdotL * light_contribution.x * diffuse_color.xyz + light_contribution.y * specular + light_contribution.z * ambient_color.xyz, float3(coeff, coeff, coeff)), 1.0);

    return output;
}