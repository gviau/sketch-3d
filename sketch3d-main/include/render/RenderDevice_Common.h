#ifndef SKETCH_3D_RENDER_DEVICE_COMMON_H
#define SKETCH_3D_RENDER_DEVICE_COMMON_H

#include "render/RenderState.h"

#include <string>
using namespace std;

namespace Sketch3D {
///////////////////////////////////////////////////////////////////////////////
// Enums
///////////////////////////////////////////////////////////////////////////////
/**
 * @enum MapFlag_t
 * Enumaration used to determine what are the flags to be used when mapping a pointer
 * to an hardware resource
 */
enum MapFlag_t {
    READ,
    WRITE,
    READ_WRITE,
    WRITE_DISCARD,
    WRITE_NO_OVERWRITE
};

/**
 * @enum ComparisonFunction_t
 * Enumeration of the different comparison functions
 */
enum ComparisonFunction_t {
    NEVER,
    LESS,
    EQUAL,
    LESS_EQUAL,
    GREATER,
    NOT_EQUAL,
    GREATER_EQUAL,
    ALWAYS
};

/**
 * @enum StencilOperation_t
 * Enumeration containing data about which operation to perform depending on the stencil result
 */
enum StencilOperation_t {
    KEEP,
    ZERO,
    REPLACE,
    INCR_STAT,
    DECR_STAT,
    INVERT,
    INCR,
    DECR
};

/**
 * @enum PrimitiveTopology_t
 * Enumerates the different way that a set of vertices can be drawn
 */
enum PrimitiveTopology_t {
    POINTLIST,
    LINELIST,
    LINESTRIP,
    TRIANGLELIST,
    TRIANGLESTRIP
};

/**
 * @enum SemanticName_t
 * Enum representing the different semantics that the vertices' attributes can refer to
 */
enum SemanticName_t {
    BINORMAL,
    BONES,
    COLOR,
    NORMAL,
    POSITION,
    TANGENT,
    TEXCOORD,
    WEIGHTS
};

/**
 * @enum InputFormat
 * Enum describing all the possible formats that can be used by the vertices' attributes
 */
enum InputFormat_t {
    BYTE_UINT1,
    BYTE_UINT2,
    BYTE_UINT4,

    BYTE_SINT1,
    BYTE_SINT2,
    BYTE_SINT4,

    HALF_FLOAT1,
    HALF_FLOAT2,
    HALF_FLOAT4,

    HALF_UINT1,
    HALF_UINT2,
    HALF_UINT4,

    HALF_SINT1,
    HALF_SINT2,
    HALF_SINT4,

    FLOAT1,
    FLOAT2,
    FLOAT3,
    FLOAT4,

    UINT1,
    UINT2,
    UINT3,
    UINT4,

    SINT1,
    SINT2,
    SINT3,
    SINT4,

    // Special types
    R10G10B10A2_UINT,
    R11G11B10_FLOAT
};

/**
 * @enum IndexFormat_t
 * The different format for an index buffer
 */
enum IndexFormat_t {
    INT_2_BYTES,
    INT_4_BYTES
};

///////////////////////////////////////////////////////////////////////////////
// Structs
///////////////////////////////////////////////////////////////////////////////
/**
 * @struct InputLayout_t
 * This structure represents the data set to describe the vertex input layout
 *  - semanticName : This represent the semantic used in the shader for this data element
 *  - semanticIndex : The index of the semantic name if there are several same semantics
 *  - format : The format of the data element
 *  - inputSlot : Used if several vertex buffers are used to specify the data elements. The input
 *                slot corresponds to the slot to which the corresponding vertex buffer is bound.
 *                It is set to 0 if using a single vertex buffer.
 *  - byteOffset : Offset in bytes between each elements. Includes any padding used
 *  - isDataPerInstance : Set to true if the data element is used per instances, for example, model
 *                        matrices. Usually false for most input layout
 *  - instanceDataStepRate : If isDataPerInstance is set to false, this must be 0. Otherwise, this
 *                           value represents the number of instances to draw using the same
 *                           per-instance data before advancing in the buffer by one element.
 */
struct InputLayout_t {
    SemanticName_t  semanticName;
    unsigned int    semanticIndex;
    InputFormat_t   format;
    unsigned int    inputSlot;
    unsigned int    byteOffset;
    bool            isDataPerInstance;
    unsigned int    instanceDataStepRate;

    bool operator!= (const InputLayout_t& rhs) {
        return (semanticName != rhs.semanticName &&
                semanticIndex != rhs.semanticIndex &&
                format != rhs.format &&
                inputSlot != rhs.inputSlot &&
                byteOffset != rhs.byteOffset &&
                isDataPerInstance != rhs.isDataPerInstance &&
                instanceDataStepRate != rhs.instanceDataStepRate);
    }
};

/**
 * @struct DepthStencilState_t
 * Structure containing the required information to set the state for the depth stencil buffer
 */
struct DepthStencilState_t {
    bool                    depthEnable;
    bool                    depthWriteMask;
    ComparisonFunction_t    comparisonFunction;
    bool                    stencilEnable;
    unsigned char           stencilReadMask;
    unsigned char           stencilWriteMask;
};

/**
 * @struct RasterizerState_t
 * Structure containing the required information to set the rasterizer state
 */
struct RasterizerState_t {
    FillMode_t      fillMode;
    CullingMethod_t cullingMethod;
    bool            isFrontFaceCounterClockwise;
    bool            enableDepthClip;
    bool            enableScissor;
    bool            enableMultisample;
    bool            enableAntialiasedLine;
};
}

#endif