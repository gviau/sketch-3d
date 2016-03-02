cbuffer PassConstants_t : register(b0)
{
    float4x4 projectionMatrix;
    float4x4 viewMatrix;
};

cbuffer DrawConstants_t : register(b1)
{
    float4x4 modelMatrix;

    float4x4 modelViewProjectionMatrix;
    float4x4 ViewProjectionMatrix;
    float4x4 modelViewMatrix;

    float4x4 inverseTransposeModelViewMatrix;
};

struct VS_INPUT
{
    float3 in_vertex : POSITION;
    float3 in_normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 normal : TEXCOORD0;
    float3 lightDir : TEXCOORD1;
    float3 eye : TEXCOORD2;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    float4x4 modelView = mul(viewMatrix, modelMatrix);
    float4x4 mvp = mul(projectionMatrix, modelView);

    output.position = mul(mvp, float4(input.in_vertex, 1.0));

    float3x3 modelView3x3 = (float3x3)modelView;
    output.normal = normalize(mul(modelView3x3, input.in_normal));

    float3 pos = mul(modelView, float4(input.in_vertex, 1.0)).xyz;
    output.lightDir = normalize(mul(viewMatrix, float4(5.0, 5.0, 4.0, 1.0)).xyz - pos);

    output.eye = normalize(-pos);

    return output;
}