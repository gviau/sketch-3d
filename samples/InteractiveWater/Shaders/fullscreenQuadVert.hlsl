struct VS_INPUT {
    float3 position : POSITION;
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.position = float4(input.position.xy, 0.0, 1.0);
    output.uv = input.position.xy * 0.5 + 0.5;
    output.uv.y = -output.uv.y;

    return output;
}