float4x4 modelViewProjection;
float4x4 modelView;
float4x4 view;

struct VS_INPUT {
    float3 in_vertex : POSITION;
    float3 in_normal : NORMAL;
    float2 in_uv : TEXCOORD0;
    float3 in_tangent : TANGENT;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float3 normal : TEXCOORD0;
    float2 uv : TEXCOORD1;
    float3 light_dir : TEXCOORD2;
    float3 eye : TEXCOORD3;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;

    // Compute the bitangent from the normal and the tangent
    float3 norm = normalize( mul( (float3x3)modelView, input.in_normal ) );
    float3 tangent = normalize( mul( (float3x3)modelView, input.in_tangent ) );
    float3 bitangent = normalize( cross(norm, tangent) );

    // Construct the TBN matrix which will transform points from the camera space to tangent space
    float3x3 tbn = transpose(float3x3(
        tangent,
        bitangent,
        norm
    ));

    float3 pos = mul( modelView, float4(input.in_vertex, 1.0) ).xyz;
    output.light_dir = normalize( mul(tbn, mul( (float3x3)view, float3(2.0, 2.0, -6.0) )) - pos );
    output.eye = mul(tbn, normalize(-pos));

    output.normal = norm;
    output.uv = input.in_uv;

    output.position = mul(modelViewProjection, float4(input.in_vertex, 1.0));

    return output;
}