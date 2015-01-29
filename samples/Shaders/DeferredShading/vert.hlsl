float4x4 modelViewProjection;

struct VS_INPUT {
    float3 in_vertex : POSITION;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float2 uv : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;

    output.uv = input.in_vertex.xy * 0.5 + 0.5;
    output.uv.y = 1.0 - output.uv.y;
    output.position = float4(input.in_vertex.xy, 0.0, 1.0);

    return output;
}