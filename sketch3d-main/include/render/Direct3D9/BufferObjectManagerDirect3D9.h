#ifndef SKETCH_3D_BUFFER_OBJECT_MANAGER_DIRECT3D9_H
#define SKETCH_3D_BUFFER_OBJECT_MANAGER_DIRECT3D9_H

#include "render/BufferObjectManager.h"

// Forward class declaration
struct IDirect3DDevice9;

namespace Sketch3D {

/**
 * @class BufferObjectManagerDirect3D9
 * Direct3D9 implementation of the buffer object manager
 */
class BufferObjectManagerDirect3D9 : public BufferObjectManager {
    public:
                                BufferObjectManagerDirect3D9(IDirect3DDevice9* device);
        virtual BufferObject*   CreateBufferObject(const VertexAttributesMap_t& vertexAttributes, BufferUsage_t usage=BUFFER_USAGE_STATIC);

    private:
        IDirect3DDevice9*       device_;
};

}

#endif