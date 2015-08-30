#ifndef SKETCH_3D_VERTEX_FORMAT_H
#define SKETCH_3D_VERTEX_FORMAT_H

#include "render/RenderDevice_Common.h"

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

#include <vector>
using namespace std;

namespace Sketch3D {

///////////////////////////////////////////////////////////////////////////////
// VERTEX TYPES
///////////////////////////////////////////////////////////////////////////////
struct Vertex_Pos_t {
    Vector3 position;
};

struct Vertex_Pos_Color_t {
    Vector3 position;
    Vector3 color;
};

struct Vertex_Pos_UV_t {
    Vector3 position;
    Vector2 uv;
};

struct Vertex_Pos_Normal_t {
    Vector3 position;
    Vector3 normal;
};

struct Vertex_Pos_UV_Normal_t {
    Vector3 position;
    Vector2 uv;
    Vector3 normal;
};

struct Vertex_Pos_UV_Normal_Tangent_t {
    Vector3 position;
    Vector2 uv;
    Vector3 normal;
    Vector3 tangent;
};

struct Vertex_Pos_UV_Normal_4_Bones_t {
    Vector3 position;
    Vector2 uv;
    Vector3 normal;
    Vector4 bones;
    Vector4 weights;
};

struct Vertex_Pos_UV_Normal_Tangent_4_Bones_t {
    Vector3 position;
    Vector2 uv;
    Vector3 normal;
    Vector3 tangent;
    Vector4 bones;
    Vector4 weights;
};

struct Vertex_Pos_UV_4_Bones_t {
    Vector3 position;
    Vector2 uv;
    Vector4 bones;
    Vector4 weights;
};

struct Vertex_Pos_Normal_4_Bones_t {
    Vector3 position;
    Vector3 normal;
    Vector4 bones;
    Vector4 weights;
};

struct Vertex_Pos_2_UV_t {
    Vector3 position;
    Vector2 uv1;
    Vector2 uv2;
};

struct Vertex_Pos_2_UV_Normal_t {
    Vector3 position;
    Vector2 uv1;
    Vector2 uv2;
    Vector3 normal;
};

struct Vertex_Pos_2_UV_Normal_Tangent_t {
    Vector3 position;
    Vector2 uv1;
    Vector2 uv2;
    Vector3 normal;
    Vector3 tangent;
};

struct Vertex_Pos_2_UV_Normal_4_Bones_t {
    Vector3 position;
    Vector2 uv1;
    Vector2 uv2;
    Vector3 normal;
    Vector4 bones;
    Vector4 weights;
};

struct Vertex_Pos_2_UV_Normal_Tangent_4_Bones_t {
    Vector3 position;
    Vector2 uv1;
    Vector2 uv2;
    Vector3 normal;
    Vector3 tangent;
    Vector4 bones;
    Vector4 weights;
};

struct Vertex_Pos_2_UV_4_Bones_t {
    Vector3 position;
    Vector2 uv1;
    Vector2 uv2;
    Vector4 bones;
    Vector4 weights;
};

///////////////////////////////////////////////////////////////////////////////
// VERTEX FORMATS
///////////////////////////////////////////////////////////////////////////////
#define NEW_INPUT_LAYOUT(semanticName, semanticIndex, format, inputSlot, byteOffset, isDataPerInstance, instanceDataStepRate) \
    { semanticName, semanticIndex, format, inputSlot, byteOffset, isDataPerInstance, instanceDataStepRate }

enum VertexFormatType_t
{
    Pos,
    Pos_Color,
    Pos_UV,
    Pos_Normal,
    Pos_UV_Normal,
    Pos_UV_Normal_Tangent,
    Pos_UV_Normal_4_Bones,
    Pos_UV_Normal_Tangent_4_Bones,
    Pos_UV_4_Bones,
    Pos_Normal_4_Bones,
    Pos_2_UV,
    Pos_2_UV_Normal,
    Pos_2_UV_Normal_Tangent,
    Pos_2_UV_Normal_4_Bones,
    Pos_2_UV_Normal_Tangent_4_Bones,
    Pos_2_UV_4_Bones,

    VertexFormatType_Max
};

/**
 * @class VertexFormat
 * Base class for the vertex format. Enforce a method to return a list of input layouts
 */
class VertexFormat {
    public:
        const vector<InputLayout_t>& GetInputLayouts() { return inputLayouts_; }
        virtual size_t GetSize() = 0;

    protected:
        vector<InputLayout_t> inputLayouts_;
};

class VertexFormat_Pos : public VertexFormat {
    public:
        VertexFormat_Pos();
        virtual size_t GetSize() override { return sizeof(Vertex_Pos_t); }
};

class VertexFormat_Pos_Color : public VertexFormat {
    public:
        VertexFormat_Pos_Color();
        virtual size_t GetSize() override { return sizeof(Vertex_Pos_Color_t); }
};

class VertexFormat_Pos_UV : public VertexFormat {
    public:
        VertexFormat_Pos_UV();
        virtual size_t GetSize() override { return sizeof(Vertex_Pos_UV_t); }
};

class VertexFormat_Pos_Normal : public VertexFormat {
    public:
        VertexFormat_Pos_Normal();
        virtual size_t GetSize() override { return sizeof(Vertex_Pos_Normal_t); }
};

class VertexFormat_Pos_UV_Normal : public VertexFormat {
    public:
        VertexFormat_Pos_UV_Normal();
        virtual size_t GetSize() override { return sizeof(Vertex_Pos_UV_Normal_t); }
};

class VertexFormat_Pos_UV_Normal_Tangent : public VertexFormat {
    public:
        VertexFormat_Pos_UV_Normal_Tangent();
        virtual size_t GetSize() override { return sizeof(Vertex_Pos_UV_Normal_Tangent_t); }
};

class VertexFormat_Pos_UV_Normal_4_Bones : public VertexFormat {
    public:
        VertexFormat_Pos_UV_Normal_4_Bones();
        virtual size_t GetSize() override { return sizeof(Vertex_Pos_UV_Normal_4_Bones_t); }
};

class VertexFormat_Pos_UV_Normal_Tangent_4_Bones : public VertexFormat {
    public:
        VertexFormat_Pos_UV_Normal_Tangent_4_Bones();
        virtual size_t GetSize() override { return sizeof(Vertex_Pos_UV_Normal_Tangent_4_Bones_t); }
};

class VertexFormat_Pos_UV_4_Bones : public VertexFormat {
    public:
        VertexFormat_Pos_UV_4_Bones();
        virtual size_t GetSize() override { return sizeof(Vertex_Pos_UV_4_Bones_t); }
};

class VertexFormat_Pos_Normal_4_Bones : public VertexFormat {
    public:
        VertexFormat_Pos_Normal_4_Bones();
        virtual size_t GetSize() override { return sizeof(Vertex_Pos_Normal_4_Bones_t); }
};

class VertexFormat_Pos_2_UV : public VertexFormat {
    public:
        VertexFormat_Pos_2_UV();
        virtual size_t GetSize() override { return sizeof(Vertex_Pos_2_UV_t); }
};

class VertexFormat_Pos_2_UV_Normal : public VertexFormat {
    public:
        VertexFormat_Pos_2_UV_Normal();
        virtual size_t GetSize() override { return sizeof(Vertex_Pos_2_UV_Normal_t); }
};

class VertexFormat_Pos_2_UV_Normal_Tangent : public VertexFormat {
    public:
        VertexFormat_Pos_2_UV_Normal_Tangent();
        virtual size_t GetSize() override { return sizeof(Vertex_Pos_2_UV_Normal_Tangent_t); }
};

class VertexFormat_Pos_2_UV_Normal_4_Bones : public VertexFormat {
    public:
        VertexFormat_Pos_2_UV_Normal_4_Bones();
        virtual size_t GetSize() override { return sizeof(Vertex_Pos_2_UV_Normal_4_Bones_t); }
};

class VertexFormat_Pos_2_UV_Normal_Tangent_4_Bones : public VertexFormat {
    public:
        VertexFormat_Pos_2_UV_Normal_Tangent_4_Bones();
        virtual size_t GetSize() override { return sizeof(Vertex_Pos_2_UV_Normal_Tangent_4_Bones_t); }
};

class VertexFormat_Pos_2_UV_4_Bones : public VertexFormat {
    public:
        VertexFormat_Pos_2_UV_4_Bones();
        virtual size_t GetSize() override { return sizeof(Vertex_Pos_2_UV_4_Bones_t); }
};

void GetVertexFormat(VertexFormatType_t vertexFormatType, VertexFormat*& vertexFormat);
bool VertexFormatTypeContainsColor(VertexFormatType_t type);
bool VertexFormatTypeContainsUV(VertexFormatType_t type);
bool VertexFormatTypeContains2UVs(VertexFormatType_t type);
bool VertexFormatTypeContainsNormals(VertexFormatType_t type);
bool VertexFormatTypeContainsTangents(VertexFormatType_t type);
bool VertexFormatTypeContainsBones(VertexFormatType_t type);

}

#endif