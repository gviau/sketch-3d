#include "render/Buffer.h"

namespace Sketch3D {
VertexFormatType_t VertexBuffer::GetVertexFormatType() const
{
    return vertexFormatType_;
}

size_t VertexBuffer::GetNumVertices() const
{
    return numVertices_;
}

IndexFormat_t IndexBuffer::GetIndexFormat() const
{
    return indexFormat_;
}

size_t IndexBuffer::GetNumIndices() const
{
    return numIndices_;
}

}