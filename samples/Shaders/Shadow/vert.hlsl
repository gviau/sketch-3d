float4x4 modelViewProjection;
float4x4 modelView;
float4x4 model;
float4x4 view;
float4x4 shadowMatrix;

float3 light_position;

struct VS_INPUT {
    float3 in_vertex : POSITION;
    float3 in_normal : NORMAL;
    float2 in_uv : TEXCOORD;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
    float3 light_dir : TEXCOORD1;
    float3 eye : TEXCOORD2;
    float3 normal : TEXCOORD3;
    float4 shadow_coord : TEXCOORD4;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;

    // Transform everything to camera space
    float3 pos = mul(modelView, float4(input.in_vertex, 1.0)).xyz;
    output.light_dir = normalize( mul(view, float4(light_position, 1.0)).xyz - pos);
    output.eye = normalize(-pos);
    output.normal = mul((float3x3)modelView, input.in_normal);

    output.uv = input.in_uv;
    output.position = mul(modelViewProjection, float4(input.in_vertex, 1.0));

    // shadowMatrix convertex from model coordinates to shadow map coordinates
    output.shadow_coord = mul( mul(shadowMatrix, model), float4(input.in_vertex, 1.0));
    //output.shadow_coord.x =  output.shadow_coord.x * 0.5 + 0.5 * output.shadow_coord.w;
    //output.shadow_coord.y =  output.shadow_coord.y * 0.5 + 0.5 * output.shadow_coord.w;

    return output;
}