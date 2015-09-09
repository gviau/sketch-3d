cbuffer PassConstants_t : register(b0) {
    float4x4 projectionMatrix;
    float4x4 viewMatrix;
};

cbuffer DrawConstants_t : register(b1) {
    float4x4 modelMatrix;

    float4x4 modelViewProjectionMatrix;
    float4x4 ViewProjectionMatrix;
    float4x4 modelViewMatrix;

    float4x4 inverseTransposeModelViewMatrix;
};

struct VS_INPUT {
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float3 normal : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.position = mul(modelMatrix, float4(input.position, 1.0));
    output.normal = input.normal;

    return output;
}