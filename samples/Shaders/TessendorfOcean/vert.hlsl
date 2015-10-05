cbuffer DrawConstants_t : register(b0)
{
    float4x4 modelMatrix;
    
    float4x4 modelViewProjectionMatrix;
    float4x4 viewProjectionMatrix;
    float4x4 modelViewMatrix;

    float4x4 inverseTransposeModelViewMatrix;
};

cbuffer OceanConstants_t : register(b1)
{
    float4 light_position;
    float4 ambient_color;
    float4 diffuse_color;
    float4 specular_color;
    float4 light_contribution;
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
    float3 light_dir : TEXCOORD1;
    float3 eye : TEXCOORD2;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    float3 pos = mul(modelViewMatrix, float4(input.in_vertex, 1.0)).xyz;

    output.normal = normalize(mul((float3x3)modelViewMatrix, input.in_normal));
    output.light_dir = normalize( mul((float3x3)modelViewMatrix, light_position.xyz) - pos);
    output.eye = normalize(-pos);

    output.position = mul(modelViewProjectionMatrix, float4(input.in_vertex, 1.0));

    return output;
}