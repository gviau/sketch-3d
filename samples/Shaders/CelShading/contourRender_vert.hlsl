float4x4 modelViewProjection;
float4x4 modelView;

float offset;

struct VS_INPUT {
    float3 in_vertex : POSITION;
    float3 in_normal : NORMAL;
};

struct VS_OUTPUT {
    float4 position : POSITION;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;

    output.position = mul(modelViewProjection, float4(input.in_vertex + input.in_normal * offset, 1.0));

    return output;
}