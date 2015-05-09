float4x4 modelViewProjection;
float4x4 boneTransformationMatrices[40];

struct VS_INPUT {
    float3 in_vertex : POSITION;
    float3 in_normal : NORMAL;
    float2 in_uv : TEXCOORD0;
    float4 in_bones : BLENDINDICES;
    float4 in_weights : BLENDWEIGHT;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float3 normal : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;

    float4x4 boneTransform = boneTransformationMatrices[(int)input.in_bones.x] * input.in_weights.x +
                             boneTransformationMatrices[(int)input.in_bones.y] * input.in_weights.y +
                             boneTransformationMatrices[(int)input.in_bones.z] * input.in_weights.z +
                             boneTransformationMatrices[(int)input.in_bones.w] * input.in_weights.w;

    float4 pos = mul(boneTransform, float4(input.in_vertex, 1.0));
        
    output.uv = input.in_uv;
    output.position = mul(modelViewProjection, pos);

    return output;
}