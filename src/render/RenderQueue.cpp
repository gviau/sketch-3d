#include "render/RenderQueue.h"

#include "render/BufferObject.h"
#include "render/Material.h"
#include "render/Mesh.h"
#include "render/Node.h"
#include "render/Renderer.h"
#include "render/Shader.h"
#include "render/SkinnedMesh.h"
#include "render/Texture2D.h"

#include <algorithm>
using namespace std;

namespace Sketch3D {

RenderQueue::RenderQueue() {
}

void RenderQueue::AddItem(const Node* node, Layer_t layer) {
    // TEMP
    // Got to change the distance from the camera
    items_.push_back(new RenderQueueItem(node, 0.0f, layer));
}

void RenderQueue::Render() {
    // Sort
    stable_sort(items_.begin(), items_.end(), RenderQueueItemKeyComparator());

    // Batch if possible

    for (size_t i = 0; i < items_.size(); i++) {
        RenderQueueItem* item = items_[i];
        item->node_->Render();
    }

    // Invalidate the render queue
    for (size_t i = 0; i < items_.size(); i++) {
        delete items_[i];
    }
    items_.clear();
}

}
