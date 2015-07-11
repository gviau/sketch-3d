#include "render/Buffer.h"

namespace Sketch3D {
Buffer::Buffer(void* initialData, bool dynamic, bool immutable) {
}

VertexBuffer::VertexBuffer(void* initialData, bool dynamic, bool immutable, VertexFormat* vertexFormat, size_t numVertices) : Buffer(initialData, dynamic, immutable), vertexFormat_(vertexFormat), numVertices_(numVertices) {
}

VertexFormat* VertexBuffer::GetVertexFormat() const {
    return vertexFormat_;
}

size_t VertexBuffer::GetNumVertices() const {
    return numVertices_;
}

IndexBuffer::IndexBuffer(void* initialData, bool dynamic, bool immutable, IndexFormat_t indexFormat, size_t numIndices) : Buffer(initialData, dynamic, immutable), indexFormat_(indexFormat), numIndices_(numIndices) {
}

IndexFormat_t IndexBuffer::GetIndexFormat() const {
    return indexFormat_;
}

size_t IndexBuffer::GetNumIndices() const {
    return numIndices_;
}

ConstantBuffer::ConstantBuffer(void* initialData, bool dynamic, bool immutable, size_t dataSize) : Buffer(initialData, dynamic, immutable) {
}

}