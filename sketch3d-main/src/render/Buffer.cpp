#include "render/Buffer.h"

namespace Sketch3D {
VertexFormat* VertexBuffer::GetVertexFormat() const {
    return vertexFormat_;
}

size_t VertexBuffer::GetNumVertices() const {
    return numVertices_;
}

IndexFormat_t IndexBuffer::GetIndexFormat() const {
    return indexFormat_;
}

size_t IndexBuffer::GetNumIndices() const {
    return numIndices_;
}

}