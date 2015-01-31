float4x4 modelViewProjection;
float4x4 modelView;

struct VS_INPUT {
    float3 in_vertex : POSITION;
    float3 in_normal : NORMAL;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float3 normal : TEXCOORD0;
    float3 light_dir : TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;

    output.normal = mul((float3x3)modelView, input.in_normal);

    float3 pos = mul(modelView, float4(input.in_vertex, 1.0)).xyz;
    output.light_dir = mul((float3x3)modelView, float3(1.0, 1.0, -3.0)) - pos;
    output.position = mul(modelViewProjection, float4(input.in_vertex, 1.0));

    return output;
}