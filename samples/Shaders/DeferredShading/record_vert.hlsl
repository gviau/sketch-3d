float4x4 modelViewProjection;
float4x4 modelView;

struct VS_INPUT {
    float3 in_vertex : POSITION;
    float3 in_normal : NORMAL;
    float2 in_uv : TEXCOORD;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float3 normal : TEXCOORD0;
    float3 pos : TEXCOORD1;
    float2 uv : TEXCOORD2;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;

    output.normal = mul((float3x3)modelView, input.in_normal);
    output.pos = mul(modelView, float4(input.in_vertex, 1.0)).xyz;
    output.uv = input.in_uv;

    output.position = mul(modelViewProjection, float4(input.in_vertex, 1.0));

    return output;
}