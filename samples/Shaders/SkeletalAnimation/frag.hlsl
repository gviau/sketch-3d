sampler texture0;

struct PS_INPUT {
    float4 position : POSITION;
    float3 normal : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

float4 main(PS_INPUT input) : COLOR {
    return float4(tex2D(texture0, input.uv).xyz, 1.0);
}