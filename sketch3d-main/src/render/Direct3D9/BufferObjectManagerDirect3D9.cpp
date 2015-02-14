#include "render/Direct3D9/BufferObjectManagerDirect3D9.h"

#include "render/Direct3D9/BufferObjectDirect3D9.h"

namespace Sketch3D {

BufferObjectManagerDirect3D9::BufferObjectManagerDirect3D9(IDirect3DDevice9* device) : BufferObjectManager(), device_(device) {
}

BufferObject* BufferObjectManagerDirect3D9::CreateBufferObject(const VertexAttributesMap_t& vertexAttributes, BufferUsage_t usage) {
    BufferObject* buffer = new BufferObjectDirect3D9(device_, vertexAttributes, usage);
    bufferObjects_.insert(buffer);
    return buffer;
}

}