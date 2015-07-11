cbuffer PassConstants_t {
    float4x4 projectionMatrix;
    float4x4 viewMatrix;
    float4x4 modelMatrix;

    float4x4 modelViewProjectionMatrix;
    float4x4 ViewProjectionMatrix;
    float4x4 modelViewMatrix;

    float4x4 inverseTransposeModelViewMatrix;
};

struct VS_INPUT {
    float3 position : POSITION;
    float3 color : COLOR;
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float3 color : COLOR;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;

    float4x4 modelViewProjection = mul(projectionMatrix, mul(viewMatrix, modelMatrix));
    output.position = mul(modelViewProjection, float4(input.position, 1.0));
    output.color = input.color;

    return output;
}