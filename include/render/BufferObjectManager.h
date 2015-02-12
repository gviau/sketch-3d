#ifndef SKETCH_3D_BUFFER_OBJECT_MANAGER_H
#define SKETCH_3D_BUFFER_OBJECT_MANAGER_H

#include "render/BufferObject.h"

#include "system/Platform.h"

#include <set>
using namespace std;

namespace Sketch3D {

/**
 * @class BufferObjectManager
 * This class acts as a manager to create and handle buffer objects that are API dependent
 */
class SKETCH_3D_API BufferObjectManager {
    public:
        /**
         * Destructor - releases all buffer objects
         */
        virtual                ~BufferObjectManager();

        /**
         * Create a buffer object
         */
        virtual BufferObject*   CreateBufferObject(const VertexAttributesMap_t& vertexAttributes, BufferUsage_t usage=BUFFER_USAGE_STATIC) = 0;

        /**
         * Delete the specified buffer object
         * @param bufferObject The buffer object to delete
         */
        void                    DeleteBufferObject(BufferObject* bufferObject);

    protected:
        set<BufferObject*>      bufferObjects_; /**< Buffer objects allocted by the manager */
};

}

#endif