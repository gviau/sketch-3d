#include "render/OpenGL/BufferObjectManagerOpenGL.h"

#include "render/OpenGL/BufferObjectOpenGL.h"

namespace Sketch3D {

BufferObject* BufferObjectManagerOpenGL::CreateBufferObject(const VertexAttributesMap_t& vertexAttributes, BufferUsage_t usage) {
    BufferObject* buffer = new BufferObjectOpenGL(vertexAttributes, usage);
    bufferObjects_.insert(buffer);
    return buffer;
}

}