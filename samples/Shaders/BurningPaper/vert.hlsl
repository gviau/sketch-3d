float4x4 modelViewProjection;

struct VS_INPUT {
    float3 in_vertex : POSITION;
    float2 in_uv : TEXCOORD;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float2 uv : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    output.position = mul(modelViewProjection, float4(input.in_vertex, 1.0));
    output.uv = input.in_uv;
    return output;
}