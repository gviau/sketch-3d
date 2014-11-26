#include "render/BufferObject.h"

namespace Sketch3D {

BufferObject::BufferObject(const VertexAttributesMap_t& vertexAttributes, BufferUsage_t usage) : vertexAttributes_(vertexAttributes), usage_(usage),
        vertexCount_(0), indexCount_(0) {
}

BufferObject::~BufferObject() {
}

}