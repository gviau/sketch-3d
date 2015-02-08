float4x4 viewProjection;
float4x4 view;

struct VS_INPUT {
    float3 in_vertex : POSITION;
    float3 in_normal : NORMAL;
    float4 in_world0 : TEXCOORD0;
    float4 in_world1 : TEXCOORD1;
    float4 in_world2 : TEXCOORD2;
    float4 in_world3 : TEXCOORD3;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float3 normal : TEXCOORD0;
    float3 light_dir : TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;

    float4x4 model = float4x4(input.in_world0,
                              input.in_world1,
                              input.in_world2,
                              float4(input.in_world3.xyz, 1.0));
    float4x4 modelView = mul(view, model);
    float4x4 modelViewProjection = mul(viewProjection, model);

    output.normal = mul((float3x3)modelView, input.in_normal);

    float3 pos = mul(modelView, float4(input.in_vertex, 1.0)).xyz;
    output.light_dir = mul((float3x3)modelView, float3(1.0, 1.0, -3.0)) - pos;
    output.position = mul(modelViewProjection, float4(input.in_vertex, 1.0));

    return output;
}