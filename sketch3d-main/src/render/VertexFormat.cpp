#include "render/VertexFormat.h"

namespace Sketch3D {

// VertexFormat creation
VertexFormat_Pos vertexFormat_Pos;
VertexFormat_Pos_Color vertexFormat_Pos_Color;
VertexFormat_Pos_UV vertexFormat_Pos_UV;
VertexFormat_Pos_Normal vertexFormat_Pos_Normal;
VertexFormat_Pos_UV_Normal vertexFormat_Pos_UV_Normal;
VertexFormat_Pos_UV_Normal_Tangent vertexFormat_Pos_UV_Normal_Tangent;
VertexFormat_Pos_UV_Normal_4_Bones vertexFormat_Pos_UV_Normal_4_Bones;
VertexFormat_Pos_UV_Normal_Tangent_4_Bones vertexFormat_Pos_UV_Normal_Tangent_4_Bones;
VertexFormat_Pos_UV_4_Bones vertexFormat_Pos_UV_4_Bones;
VertexFormat_Pos_Normal_4_Bones vertexFormat_Pos_Normal_4_Bones;
VertexFormat_Pos_2_UV vertexFormat_Pos_2_UV;
VertexFormat_Pos_2_UV_Normal vertexFormat_Pos_2_UV_Normal;
VertexFormat_Pos_2_UV_Normal_Tangent vertexFormat_Pos_2_UV_Normal_Tangent;
VertexFormat_Pos_2_UV_Normal_4_Bones vertexFormat_Pos_2_UV_Normal_4_Bones;
VertexFormat_Pos_2_UV_Normal_Tangent_4_Bones vertexFormat_Pos_2_UV_Normal_Tangent_4_Bones;
VertexFormat_Pos_2_UV_4_Bones vertexFormat_Pos_2_UV_4_Bones;

VertexFormat_Pos::VertexFormat_Pos() {
    InputLayout_t position = NEW_INPUT_LAYOUT(SemanticName_t::POSITION, 0, InputFormat_t::FLOAT3, 0, 0, false, 0);

    inputLayouts_.push_back(position);
}

VertexFormat_Pos_Color::VertexFormat_Pos_Color() {
    InputLayout_t position  = NEW_INPUT_LAYOUT(SemanticName_t::POSITION, 0, InputFormat_t::FLOAT3, 0,  0, false, 0);
    InputLayout_t color     = NEW_INPUT_LAYOUT(SemanticName_t::COLOR,    0, InputFormat_t::FLOAT3, 0, 12, false, 0);

    inputLayouts_.push_back(position);
    inputLayouts_.push_back(color);
}

VertexFormat_Pos_UV::VertexFormat_Pos_UV() {
    InputLayout_t position  = NEW_INPUT_LAYOUT(SemanticName_t::POSITION, 0, InputFormat_t::FLOAT3, 0,  0, false, 0);
    InputLayout_t uv        = NEW_INPUT_LAYOUT(SemanticName_t::TEXCOORD, 0, InputFormat_t::FLOAT2, 0, 12, false, 0);

    inputLayouts_.push_back(position);
    inputLayouts_.push_back(uv);
}

VertexFormat_Pos_Normal::VertexFormat_Pos_Normal() {
    InputLayout_t position  = NEW_INPUT_LAYOUT(SemanticName_t::POSITION, 0, InputFormat_t::FLOAT3, 0,  0, false, 0);
    InputLayout_t normal    = NEW_INPUT_LAYOUT(SemanticName_t::NORMAL,   0, InputFormat_t::FLOAT3, 0, 12, false, 0);

    inputLayouts_.push_back(position);
    inputLayouts_.push_back(normal);
}

VertexFormat_Pos_UV_Normal::VertexFormat_Pos_UV_Normal() {
    InputLayout_t position  = NEW_INPUT_LAYOUT(SemanticName_t::POSITION, 0, InputFormat_t::FLOAT3, 0,  0, false, 0);
    InputLayout_t uv        = NEW_INPUT_LAYOUT(SemanticName_t::TEXCOORD, 0, InputFormat_t::FLOAT2, 0, 12, false, 0);
    InputLayout_t normal    = NEW_INPUT_LAYOUT(SemanticName_t::NORMAL,   0, InputFormat_t::FLOAT3, 0, 20, false, 0);

    inputLayouts_.push_back(position);
    inputLayouts_.push_back(uv);
    inputLayouts_.push_back(normal);
}

VertexFormat_Pos_UV_Normal_Tangent::VertexFormat_Pos_UV_Normal_Tangent() {
    InputLayout_t position  = NEW_INPUT_LAYOUT(SemanticName_t::POSITION, 0, InputFormat_t::FLOAT3, 0,  0, false, 0);
    InputLayout_t uv        = NEW_INPUT_LAYOUT(SemanticName_t::TEXCOORD, 0, InputFormat_t::FLOAT2, 0, 12, false, 0);
    InputLayout_t normal    = NEW_INPUT_LAYOUT(SemanticName_t::NORMAL,   0, InputFormat_t::FLOAT3, 0, 20, false, 0);
    InputLayout_t tangent   = NEW_INPUT_LAYOUT(SemanticName_t::TANGENT,  0, InputFormat_t::FLOAT3, 0, 32, false, 0);

    inputLayouts_.push_back(position);
    inputLayouts_.push_back(uv);
    inputLayouts_.push_back(normal);
    inputLayouts_.push_back(tangent);
}

VertexFormat_Pos_UV_Normal_4_Bones::VertexFormat_Pos_UV_Normal_4_Bones() {
    InputLayout_t position  = NEW_INPUT_LAYOUT(SemanticName_t::POSITION, 0, InputFormat_t::FLOAT3, 0,  0, false, 0);
    InputLayout_t uv        = NEW_INPUT_LAYOUT(SemanticName_t::TEXCOORD, 0, InputFormat_t::FLOAT2, 0, 12, false, 0);
    InputLayout_t normal    = NEW_INPUT_LAYOUT(SemanticName_t::NORMAL,   0, InputFormat_t::FLOAT3, 0, 20, false, 0);
    InputLayout_t bones     = NEW_INPUT_LAYOUT(SemanticName_t::BONES,    0, InputFormat_t::FLOAT4, 0, 32, false, 0);
    InputLayout_t weights   = NEW_INPUT_LAYOUT(SemanticName_t::WEIGHTS,  0, InputFormat_t::FLOAT4, 0, 48, false, 0);

    inputLayouts_.push_back(position);
    inputLayouts_.push_back(uv);
    inputLayouts_.push_back(normal);
    inputLayouts_.push_back(bones);
    inputLayouts_.push_back(weights);
}

VertexFormat_Pos_UV_Normal_Tangent_4_Bones::VertexFormat_Pos_UV_Normal_Tangent_4_Bones() {
    InputLayout_t position  = NEW_INPUT_LAYOUT(SemanticName_t::POSITION, 0, InputFormat_t::FLOAT3, 0,  0, false, 0);
    InputLayout_t uv        = NEW_INPUT_LAYOUT(SemanticName_t::TEXCOORD, 0, InputFormat_t::FLOAT2, 0, 12, false, 0);
    InputLayout_t normal    = NEW_INPUT_LAYOUT(SemanticName_t::NORMAL,   0, InputFormat_t::FLOAT3, 0, 20, false, 0);
    InputLayout_t tangent   = NEW_INPUT_LAYOUT(SemanticName_t::TANGENT,  0, InputFormat_t::FLOAT3, 0, 32, false, 0);
    InputLayout_t bones     = NEW_INPUT_LAYOUT(SemanticName_t::BONES,    0, InputFormat_t::FLOAT4, 0, 44, false, 0);
    InputLayout_t weights   = NEW_INPUT_LAYOUT(SemanticName_t::WEIGHTS,  0, InputFormat_t::FLOAT4, 0, 60, false, 0);

    inputLayouts_.push_back(position);
    inputLayouts_.push_back(uv);
    inputLayouts_.push_back(normal);
    inputLayouts_.push_back(tangent);
    inputLayouts_.push_back(bones);
    inputLayouts_.push_back(weights);
}

VertexFormat_Pos_UV_4_Bones::VertexFormat_Pos_UV_4_Bones() {
    InputLayout_t position  = NEW_INPUT_LAYOUT(SemanticName_t::POSITION, 0, InputFormat_t::FLOAT3, 0,  0, false, 0);
    InputLayout_t uv        = NEW_INPUT_LAYOUT(SemanticName_t::TEXCOORD, 0, InputFormat_t::FLOAT2, 0, 12, false, 0);
    InputLayout_t bones     = NEW_INPUT_LAYOUT(SemanticName_t::BONES,    0, InputFormat_t::FLOAT4, 0, 20, false, 0);
    InputLayout_t weights   = NEW_INPUT_LAYOUT(SemanticName_t::WEIGHTS,  0, InputFormat_t::FLOAT4, 0, 36, false, 0);

    inputLayouts_.push_back(position);
    inputLayouts_.push_back(uv);
    inputLayouts_.push_back(bones);
    inputLayouts_.push_back(weights);
}

VertexFormat_Pos_Normal_4_Bones::VertexFormat_Pos_Normal_4_Bones() {
    InputLayout_t position  = NEW_INPUT_LAYOUT(SemanticName_t::POSITION, 0, InputFormat_t::FLOAT3, 0,  0, false, 0);
    InputLayout_t normal    = NEW_INPUT_LAYOUT(SemanticName_t::NORMAL,   0, InputFormat_t::FLOAT3, 0, 12, false, 0);
    InputLayout_t bones     = NEW_INPUT_LAYOUT(SemanticName_t::BONES,    0, InputFormat_t::FLOAT4, 0, 24, false, 0);
    InputLayout_t weights   = NEW_INPUT_LAYOUT(SemanticName_t::WEIGHTS,  0, InputFormat_t::FLOAT4, 0, 40, false, 0);

    inputLayouts_.push_back(position);
    inputLayouts_.push_back(normal);
    inputLayouts_.push_back(bones);
    inputLayouts_.push_back(weights);
}

VertexFormat_Pos_2_UV::VertexFormat_Pos_2_UV()
{
    InputLayout_t position  = NEW_INPUT_LAYOUT(SemanticName_t::POSITION, 0, InputFormat_t::FLOAT3, 0,  0, false, 0);
    InputLayout_t uv1       = NEW_INPUT_LAYOUT(SemanticName_t::TEXCOORD, 0, InputFormat_t::FLOAT2, 0, 12, false, 0);
    InputLayout_t uv2       = NEW_INPUT_LAYOUT(SemanticName_t::TEXCOORD, 1, InputFormat_t::FLOAT2, 0, 20, false, 0);

    inputLayouts_.push_back(position);
    inputLayouts_.push_back(uv1);
    inputLayouts_.push_back(uv2);
}

VertexFormat_Pos_2_UV_Normal::VertexFormat_Pos_2_UV_Normal() {
    InputLayout_t position  = NEW_INPUT_LAYOUT(SemanticName_t::POSITION, 0, InputFormat_t::FLOAT3, 0,  0, false, 0);
    InputLayout_t uv1       = NEW_INPUT_LAYOUT(SemanticName_t::TEXCOORD, 0, InputFormat_t::FLOAT2, 0, 12, false, 0);
    InputLayout_t uv2       = NEW_INPUT_LAYOUT(SemanticName_t::TEXCOORD, 1, InputFormat_t::FLOAT2, 0, 20, false, 0);
    InputLayout_t normal    = NEW_INPUT_LAYOUT(SemanticName_t::NORMAL,   0, InputFormat_t::FLOAT3, 0, 28, false, 0);

    inputLayouts_.push_back(position);
    inputLayouts_.push_back(uv1);
    inputLayouts_.push_back(uv2);
    inputLayouts_.push_back(normal);
}

VertexFormat_Pos_2_UV_Normal_Tangent::VertexFormat_Pos_2_UV_Normal_Tangent() {
    InputLayout_t position  = NEW_INPUT_LAYOUT(SemanticName_t::POSITION, 0, InputFormat_t::FLOAT3, 0,  0, false, 0);
    InputLayout_t uv1       = NEW_INPUT_LAYOUT(SemanticName_t::TEXCOORD, 0, InputFormat_t::FLOAT2, 0, 12, false, 0);
    InputLayout_t uv2       = NEW_INPUT_LAYOUT(SemanticName_t::TEXCOORD, 1, InputFormat_t::FLOAT2, 0, 20, false, 0);
    InputLayout_t normal    = NEW_INPUT_LAYOUT(SemanticName_t::NORMAL,   0, InputFormat_t::FLOAT3, 0, 28, false, 0);
    InputLayout_t tangent   = NEW_INPUT_LAYOUT(SemanticName_t::TANGENT,  0, InputFormat_t::FLOAT3, 0, 40, false, 0);

    inputLayouts_.push_back(position);
    inputLayouts_.push_back(uv1);
    inputLayouts_.push_back(uv2);
    inputLayouts_.push_back(normal);
    inputLayouts_.push_back(tangent);
}

VertexFormat_Pos_2_UV_Normal_4_Bones::VertexFormat_Pos_2_UV_Normal_4_Bones() {
    InputLayout_t position  = NEW_INPUT_LAYOUT(SemanticName_t::POSITION, 0, InputFormat_t::FLOAT3, 0,  0, false, 0);
    InputLayout_t uv1       = NEW_INPUT_LAYOUT(SemanticName_t::TEXCOORD, 0, InputFormat_t::FLOAT2, 0, 12, false, 0);
    InputLayout_t uv2       = NEW_INPUT_LAYOUT(SemanticName_t::TEXCOORD, 1, InputFormat_t::FLOAT2, 0, 20, false, 0);
    InputLayout_t normal    = NEW_INPUT_LAYOUT(SemanticName_t::NORMAL,   0, InputFormat_t::FLOAT3, 0, 28, false, 0);
    InputLayout_t bones     = NEW_INPUT_LAYOUT(SemanticName_t::BONES,    0, InputFormat_t::FLOAT4, 0, 40, false, 0);
    InputLayout_t weights   = NEW_INPUT_LAYOUT(SemanticName_t::WEIGHTS,  0, InputFormat_t::FLOAT4, 0, 56, false, 0);

    inputLayouts_.push_back(position);
    inputLayouts_.push_back(uv1);
    inputLayouts_.push_back(uv2);
    inputLayouts_.push_back(normal);
    inputLayouts_.push_back(bones);
    inputLayouts_.push_back(weights);
}

VertexFormat_Pos_2_UV_Normal_Tangent_4_Bones::VertexFormat_Pos_2_UV_Normal_Tangent_4_Bones() {
    InputLayout_t position  = NEW_INPUT_LAYOUT(SemanticName_t::POSITION, 0, InputFormat_t::FLOAT3, 0,  0, false, 0);
    InputLayout_t uv1       = NEW_INPUT_LAYOUT(SemanticName_t::TEXCOORD, 0, InputFormat_t::FLOAT2, 0, 12, false, 0);
    InputLayout_t uv2       = NEW_INPUT_LAYOUT(SemanticName_t::TEXCOORD, 1, InputFormat_t::FLOAT2, 0, 20, false, 0);
    InputLayout_t normal    = NEW_INPUT_LAYOUT(SemanticName_t::NORMAL,   0, InputFormat_t::FLOAT3, 0, 28, false, 0);
    InputLayout_t tangent   = NEW_INPUT_LAYOUT(SemanticName_t::TANGENT,  0, InputFormat_t::FLOAT3, 0, 40, false, 0);
    InputLayout_t bones     = NEW_INPUT_LAYOUT(SemanticName_t::BONES,    0, InputFormat_t::FLOAT4, 0, 52, false, 0);
    InputLayout_t weights   = NEW_INPUT_LAYOUT(SemanticName_t::WEIGHTS,  0, InputFormat_t::FLOAT4, 0, 68, false, 0);

    inputLayouts_.push_back(position);
    inputLayouts_.push_back(uv1);
    inputLayouts_.push_back(uv2);
    inputLayouts_.push_back(normal);
    inputLayouts_.push_back(tangent);
    inputLayouts_.push_back(bones);
    inputLayouts_.push_back(weights);
}

VertexFormat_Pos_2_UV_4_Bones::VertexFormat_Pos_2_UV_4_Bones() {
    InputLayout_t position  = NEW_INPUT_LAYOUT(SemanticName_t::POSITION, 0, InputFormat_t::FLOAT3, 0,  0, false, 0);
    InputLayout_t uv1       = NEW_INPUT_LAYOUT(SemanticName_t::TEXCOORD, 0, InputFormat_t::FLOAT2, 0, 12, false, 0);
    InputLayout_t uv2       = NEW_INPUT_LAYOUT(SemanticName_t::TEXCOORD, 1, InputFormat_t::FLOAT2, 0, 20, false, 0);
    InputLayout_t bones     = NEW_INPUT_LAYOUT(SemanticName_t::BONES,    0, InputFormat_t::FLOAT4, 0, 28, false, 0);
    InputLayout_t weights   = NEW_INPUT_LAYOUT(SemanticName_t::WEIGHTS,  0, InputFormat_t::FLOAT4, 0, 44, false, 0);

    inputLayouts_.push_back(position);
    inputLayouts_.push_back(uv1);
    inputLayouts_.push_back(uv2);
    inputLayouts_.push_back(bones);
    inputLayouts_.push_back(weights);
}

void GetVertexFormat(VertexFormatType_t vertexFormatType, VertexFormat*& vertexFormat)
{
    switch (vertexFormatType)
    {
    case VertexFormatType_t::Pos:                               vertexFormat = &vertexFormat_Pos; break;
    case VertexFormatType_t::Pos_Color:                         vertexFormat = &vertexFormat_Pos_Color; break;
    case VertexFormatType_t::Pos_UV:                            vertexFormat = &vertexFormat_Pos_UV; break;
    case VertexFormatType_t::Pos_Normal:                        vertexFormat = &vertexFormat_Pos_Normal; break;
    case VertexFormatType_t::Pos_UV_Normal:                     vertexFormat = &vertexFormat_Pos_UV_Normal; break;
    case VertexFormatType_t::Pos_UV_Normal_Tangent:             vertexFormat = &vertexFormat_Pos_UV_Normal_Tangent; break;
    case VertexFormatType_t::Pos_UV_Normal_4_Bones:             vertexFormat = &vertexFormat_Pos_UV_Normal_4_Bones; break;
    case VertexFormatType_t::Pos_UV_Normal_Tangent_4_Bones:     vertexFormat = &vertexFormat_Pos_UV_Normal_Tangent_4_Bones; break;
    case VertexFormatType_t::Pos_UV_4_Bones:                    vertexFormat = &vertexFormat_Pos_UV_4_Bones; break;
    case VertexFormatType_t::Pos_Normal_4_Bones:                vertexFormat = &vertexFormat_Pos_Normal_4_Bones; break;
    case VertexFormatType_t::Pos_2_UV:                          vertexFormat = &vertexFormat_Pos_2_UV; break;
    case VertexFormatType_t::Pos_2_UV_Normal:                   vertexFormat = &vertexFormat_Pos_2_UV_Normal; break;
    case VertexFormatType_t::Pos_2_UV_Normal_Tangent:           vertexFormat = &vertexFormat_Pos_2_UV_Normal_Tangent; break;
    case VertexFormatType_t::Pos_2_UV_Normal_4_Bones:           vertexFormat = &vertexFormat_Pos_2_UV_Normal_4_Bones; break;
    case VertexFormatType_t::Pos_2_UV_Normal_Tangent_4_Bones:   vertexFormat = &vertexFormat_Pos_2_UV_Normal_Tangent_4_Bones; break;
    case VertexFormatType_t::Pos_2_UV_4_Bones:                  vertexFormat = &vertexFormat_Pos_2_UV_4_Bones; break;
    }
}

bool VertexFormatTypeContainsColor(VertexFormatType_t type)
{
    return (type == VertexFormatType_t::Pos_Color);
}

bool VertexFormatTypeContainsUV(VertexFormatType_t type)
{
    switch (type)
    {
    case VertexFormatType_t::Pos_UV:
    case VertexFormatType_t::Pos_UV_Normal:
    case VertexFormatType_t::Pos_UV_Normal_Tangent:
    case VertexFormatType_t::Pos_UV_Normal_4_Bones:
    case VertexFormatType_t::Pos_UV_Normal_Tangent_4_Bones:
    case VertexFormatType_t::Pos_UV_4_Bones:
    case VertexFormatType_t::Pos_Normal_4_Bones:
    case VertexFormatType_t::Pos_2_UV:
    case VertexFormatType_t::Pos_2_UV_Normal:
    case VertexFormatType_t::Pos_2_UV_Normal_Tangent:
    case VertexFormatType_t::Pos_2_UV_Normal_4_Bones:
    case VertexFormatType_t::Pos_2_UV_Normal_Tangent_4_Bones:
    case VertexFormatType_t::Pos_2_UV_4_Bones:
        return true;
    }

    return false;
}

bool VertexFormatTypeContains2UVs(VertexFormatType_t type)
{
    switch (type)
    {
    case VertexFormatType_t::Pos_2_UV:
    case VertexFormatType_t::Pos_2_UV_Normal:
    case VertexFormatType_t::Pos_2_UV_Normal_Tangent:
    case VertexFormatType_t::Pos_2_UV_Normal_4_Bones:
    case VertexFormatType_t::Pos_2_UV_Normal_Tangent_4_Bones:
    case VertexFormatType_t::Pos_2_UV_4_Bones:
        return true;
    }

    return false;
}

bool VertexFormatTypeContainsNormals(VertexFormatType_t type)
{
    switch (type)
    {
    case VertexFormatType_t::Pos_UV_Normal:
    case VertexFormatType_t::Pos_UV_Normal_Tangent:
    case VertexFormatType_t::Pos_UV_Normal_4_Bones:
    case VertexFormatType_t::Pos_UV_Normal_Tangent_4_Bones:
    case VertexFormatType_t::Pos_Normal_4_Bones:
    case VertexFormatType_t::Pos_2_UV_Normal:
    case VertexFormatType_t::Pos_2_UV_Normal_Tangent:
    case VertexFormatType_t::Pos_2_UV_Normal_4_Bones:
    case VertexFormatType_t::Pos_2_UV_Normal_Tangent_4_Bones:
        return true;
    }

    return false;
}

bool VertexFormatTypeContainsTangents(VertexFormatType_t type)
{
    switch (type)
    {
    case VertexFormatType_t::Pos_UV_Normal_Tangent:
    case VertexFormatType_t::Pos_UV_Normal_Tangent_4_Bones:
    case VertexFormatType_t::Pos_2_UV_Normal_Tangent:
    case VertexFormatType_t::Pos_2_UV_Normal_Tangent_4_Bones:
        return true;
    }

    return false;
}

bool VertexFormatTypeContainsBones(VertexFormatType_t type)
{
    switch (type)
    {
    case VertexFormatType_t::Pos_UV_Normal_4_Bones:
    case VertexFormatType_t::Pos_UV_Normal_Tangent_4_Bones:
    case VertexFormatType_t::Pos_UV_4_Bones:
    case VertexFormatType_t::Pos_Normal_4_Bones:
    case VertexFormatType_t::Pos_2_UV_Normal_4_Bones:
    case VertexFormatType_t::Pos_2_UV_Normal_Tangent_4_Bones:
    case VertexFormatType_t::Pos_2_UV_4_Bones:
        return true;
    }

    return false;
}

}