sampler texture0;
sampler shadowMap;

float3 light_position;
float4 light_color;
float2 shadowMapTexelStep;

struct PS_INPUT {
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
    float3 light_dir : TEXCOORD1;
    float3 eye : TEXCOORD2;
    float3 normal : TEXCOORD3;
    float4 shadow_coord : TEXCOORD4;
};

float4 main(PS_INPUT input) : COLOR {
    float3 L = normalize(input.light_dir);
    float3 V = normalize(input.eye);
    float3 N = normalize(input.normal);

    float shadow = 0.0;
    float3 light = float3(0.0, 0.0, 0.0);

    // Compute diffuse and specular color
    float lambert = max(dot(N, L), 0.0);

    if (lambert > 0.0) {
        light += lambert * light_color.xyz;

        float3 H = normalize(V + L);
        light += pow(max(dot(H, N), 0.0), light_color.w) * light_color.xyz;
    }

    // Apply Percentage Close Filtering (PCF) on the shadow
    float3 shadowCoord = input.shadow_coord.xyz;
    shadowCoord.y = 1.0 - shadowCoord.y;
    shadowCoord /= input.shadow_coord.w;
    
    /*
    shadow += tex2D(shadowMap, shadowCoord.xy + float2(-shadowMapTexelStep.x, -shadowMapTexelStep.y)).x < shadowCoord.z ? 0.0f : 1.0f;
    shadow += tex2D(shadowMap, shadowCoord.xy + float2(-shadowMapTexelStep.x,  shadowMapTexelStep.y)).x < shadowCoord.z ? 0.0f : 1.0f;
    shadow += tex2D(shadowMap, shadowCoord.xy + float2( shadowMapTexelStep.x,  shadowMapTexelStep.y)).x < shadowCoord.z ? 0.0f : 1.0f;
    shadow += tex2D(shadowMap, shadowCoord.xy + float2( shadowMapTexelStep.x, -shadowMapTexelStep.y)).x < shadowCoord.z ? 0.0f : 1.0f;
    shadow *= 0.25;
    */
    shadow = tex2D(shadowMap, shadowCoord.xy).x < shadowCoord.z ? 0.0 : 1.0;

    // Gamma correction
    float3 textureColor = pow(tex2D(texture0, input.uv).xyz, float3(1.0/2.2, 1.0/2.2, 1.0/2.2));
    return float4( pow( textureColor * light * shadow + float3(0.1, 0.1, 0.1), float3(2.2, 2.2, 2.2)), 10);
}