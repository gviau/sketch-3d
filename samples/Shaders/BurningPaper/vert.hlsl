cbuffer DrawConstants_t : register(b0) {
    float4x4 modelMatrix;

    float4x4 modelViewProjectionMatrix;
    float4x4 modelViewMatrix;

    float4x4 transposedInverseModelViewMatrix;
};

struct VS_INPUT {
    float3 in_vertex : POSITION;
    float2 in_uv : TEXCOORD;
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    output.position = mul(modelViewProjectionMatrix, float4(input.in_vertex, 1.0));
    output.uv = input.in_uv;
    return output;
}