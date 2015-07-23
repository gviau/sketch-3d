cbuffer PassConstants_t {
    float4x4 projectionMatrix;
    float4x4 viewMatrix;
};

cbuffer DrawConstants_t {
    float4x4 modelMatrix;

    float4x4 modelViewProjectionMatrix;
    float4x4 ViewProjectionMatrix;
    float4x4 modelViewMatrix;

    float4x4 inverseTransposeModelViewMatrix;
};

cbuffer ShadowConstants_t {
    float4x4 lightViewMatrix;
    float4x4 lightProjectionMatrix;
};

struct VS_INPUT {
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 shadowCoord : TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;

    float4x4 modelViewProjection = mul(projectionMatrix, mul(viewMatrix, modelMatrix));
    output.position = mul(modelViewProjection, input.position);
    output.uv = input.uv;

    float4x4 shadowMatrix = mul(lightProjectionMatrix, mul(lightViewMatrix, modelMatrix));
    float4 shadowCoord = mul(shadowMatrix, input.position);
    shadowCoord.xy = shadowCoord.xy * 0.5 + 0.5;
    shadowCoord.y = 1.0 - shadowCoord.y;
    output.shadowCoord = shadowCoord;

    return output;
}