struct VS_INPUT
{
    float3 in_vertex : POSITION;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = float4(input.in_vertex, 1.0);
    output.uv = input.in_vertex.xy * 0.5 + 0.5;

    return output;
}