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

    float4 vertex = float4(input.in_vertex, 1.0);
    float4 hpos = mul(modelViewMatrix, vertex);
    float3 pos = hpos.xyz;

    float3x3 modelView = (float3x3)modelViewMatrix;
    float3 n = input.in_normal;

    float3 lightPos = light_position.xyz;

    output.normal = normalize(mul(modelView, n));
    output.light_dir = normalize( mul(modelView, lightPos) - pos);
    output.eye = normalize(-pos);

    output.position = mul(modelViewProjectionMatrix, vertex);

    return output;
}