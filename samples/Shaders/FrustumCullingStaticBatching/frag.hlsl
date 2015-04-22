struct PS_INPUT {
    float4 position : POSITION;
    float3 normal : TEXCOORD0;
    float3 light_dir : TEXCOORD1;
};

float4 main(PS_INPUT input) : COLOR {
    float4 color = float4(0.0, 0.0, 0.0, 1.0);

    float3 N = normalize(input.normal);
    float3 L = normalize(input.light_dir);

    float lambert = max(dot(N, L), 0.0);
    if (lambert > 0.0) {
        color.rgb = float3(lambert, lambert, lambert);
    }

    return color;
}