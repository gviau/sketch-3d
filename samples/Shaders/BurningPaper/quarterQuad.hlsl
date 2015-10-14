struct VS_INPUT
{
    float3 in_position : POSITION;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

float2 pixelOffset = float2( 0.001953195, 0.00260416666 );

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    output.uv = input.in_position.xy * 0.5 + 0.5;
    output.position = float4(input.in_position.xy, 0.0, 1.0);

    return output;
}