#include "render/BufferObjectManager.h"

#include "render/BufferObject.h"

namespace Sketch3D {

BufferObjectManager::~BufferObjectManager() {
    for (set<BufferObject*>::iterator it = bufferObjects_.begin(); it != bufferObjects_.end(); ++it) {
        delete *it;
    }
}

void BufferObjectManager::DeleteBufferObject(BufferObject* bufferObject) {
    set<BufferObject*>::iterator it = bufferObjects_.find(bufferObject);
    if (it != bufferObjects_.end()) {
        delete *it;
        bufferObjects_.erase(it);
    }
}

}