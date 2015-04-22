float4x4 viewProjection;
float4x4 view;

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
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.normal = mul( (float3x3)view, input.in_normal );

    float3 pos = mul(view, float4(input.in_vertex, 1.0)).xyz;
    output.light_dir = mul(view, float4(1.0, 1.0, -3.0, 1.0)).xyz - pos;
    output.position = mul(viewProjection, float4(input.in_vertex, 1.0));

    return output;
};