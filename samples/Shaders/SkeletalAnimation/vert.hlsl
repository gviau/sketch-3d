float4x4 modelViewProjection;

struct VS_INPUT {
    float3 in_vertex : POSITION;
    float3 in_normal : NORMAL;
    float2 in_uv : TEXCOORD;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float3 normal : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.uv = input.in_uv;
    output.position = mul(modelViewProjection, float4(input.in_vertex, 1.0));

    return output;
}