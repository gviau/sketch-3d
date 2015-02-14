#include "render/BufferObject.h"

namespace Sketch3D {

size_t BufferObject::nextAvailableId_ = 0;

BufferObject::BufferObject(const VertexAttributesMap_t& vertexAttributes, BufferUsage_t usage) : vertexAttributes_(vertexAttributes), usage_(usage),
        vertexCount_(0), indexCount_(0)
{
    id_ = nextAvailableId_++;
}

BufferObject::~BufferObject() {
}

size_t BufferObject::GetId() const {
    return id_;
}
}