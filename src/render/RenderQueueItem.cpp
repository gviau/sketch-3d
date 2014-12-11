#include "render/RenderQueueItem.h"

#include "render/Material.h"
#include "render/Node.h"
#include "render/Shader.h"
#include "render/Texture2D.h"

#include "math/Matrix3x3.h"
#include "math/Matrix4x4.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

namespace Sketch3D {

// Constants for bitshifts
const int LAYER_SHIFT = 62;
const int TRANSLUENCY_SHIFT = 60;
const int DEPTH_SHIFT = 30;
const int MATERIAL_SHIFT = 0;

const uint32_t DISTANCE_TRUNCATION = 0xFFFFFFFC;

RenderQueueItem::RenderQueueItem(const Node* node, float distanceFromCamera, Layer_t layer) : layer_(layer), transluencyType_(TRANSLUENCY_TYPE_OPAQUE),
        distanceFromCamera_(-distanceFromCamera), materialId_(0), node_(node)
{
    transluencyType_ = node_->GetMaterial()->GetTransluencyType();
    materialId_ = ConstructMaterialId();
}

RenderQueueItem::~RenderQueueItem() {
}

uint32_t RenderQueueItem::ConstructMaterialId() const {
    uint16_t shaderId = node_->GetMaterial()->GetShader()->GetId();
    return shaderId;
}

bool RenderQueueItemKeyComparator::operator() (const RenderQueueItem* x, const RenderQueueItem* y) const {
    // For opaque objects, we sort by material first to minimize state changes. Otherwise,
    // we sort by depth first, and back-to-front.
    
    if (x->layer_ > y->layer_) {
        return false;
    } else if (x->layer_ == y->layer_) {
        if (x->transluencyType_ > y->transluencyType_) {
            return false;
        } else if (x->transluencyType_ == y->transluencyType_) {
            TransluencyType_t transluency = x->transluencyType_;

            switch (transluency) {
                case TRANSLUENCY_TYPE_OPAQUE:
                    if (x->materialId_ > y->materialId_) {
                        return false;
                    } else if (x->materialId_ == y->materialId_) {
                        if (x->distanceFromCamera_ >= y->distanceFromCamera_) {
                            return false;
                        }
                    }
                    break;
                case TRANSLUENCY_TYPE_ADDITIVE:
                case TRANSLUENCY_TYPE_SUBTRACTIVE:
                    if (x->distanceFromCamera_ < y->distanceFromCamera_) {
                        return false;
                    } else if (x->distanceFromCamera_ == y->distanceFromCamera_) {
                        if (x->materialId_ >= y->materialId_) {
                            return false;
                        }
                    }
                    break;
            }
        }
    }

    return true;
}

}
