#ifndef SKETCH_3D_BUFFER_OBJECT_MANAGER_OPENGL_H
#define SKETCH_3D_BUFFER_OBJECT_MANAGER_OPENGL_H

#include "render/BufferObjectManager.h"

namespace Sketch3D {

/**
 * @class BufferObjectManagerOpenGL
 * OpenGL implementation of the buffer object manager
 */
class BufferObjectManagerOpenGL : public BufferObjectManager {
    public:
        virtual BufferObject* CreateBufferObject(const VertexAttributesMap_t& vertexAttributes, BufferUsage_t usage=BUFFER_USAGE_STATIC);
};

}

#endif