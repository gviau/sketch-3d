struct PS_INPUT {
    float4 position : POSITION;
    float3 normal : NORMAL;
    float3 light_dir : TEXCOORD0;
};

float4 main(PS_INPUT input) : COLOR {
    float3 N = normalize(input.normal);
    float3 L = normalize(input.light_dir);

    float lambert = max(dot(N, L), 0.0);

    float4 color;
    color.rgb = (lambert > 0.95 ? float3(1.0, 0.5, 0.5) :
                 lambert > 0.5 ? float3(0.6, 0.3, 0.3) :
                 lambert > 0.25 ? float3(0.4, 0.2, 0.2) :
                 float3(0.2, 0.1, 0.1));
    color.a = 1.0;

    return color;
}