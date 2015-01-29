sampler texture0;

float3 light_diffuse;
float4 light_specular;

struct PS_INPUT {
    float4 position : POSITION;
    float3 normal : TEXCOORD0;
    float3 pos : TEXCOORD1;
    float2 uv : TEXCOORD2;
};

struct PS_OUTPUT {
    float4 position : COLOR0;
    float4 norm : COLOR1;
    float4 albedo : COLOR2;
};

PS_OUTPUT main(PS_INPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;

    output.position.xyz = input.pos;
    output.norm.xyz = input.normal;
    output.albedo.xyz = tex2D(texture0, input.uv).xyz;

    return output;
}