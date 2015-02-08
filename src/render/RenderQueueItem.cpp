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
const int DEPTH_SHIFT = 30;
const int MATERIAL_SHIFT = 32;

const uint32_t DISTANCE_TRUNCATION = 0xFFFFFFFF;

RenderQueueItem::RenderQueueItem(shared_ptr<Matrix4x4> modelMatrix, Material* material, Texture2D** textures,
                                 size_t numTextures, BufferObject* bufferObject, bool useInstancing, uint32_t distanceFromCamera, Layer_t layer) : key_(0),
        layer_(layer), transluencyType_(TRANSLUENCY_TYPE_OPAQUE), distanceFromCamera_(distanceFromCamera), materialId_(0), modelMatrix_(modelMatrix),
        material_(material), textures_(textures), numTextures_(numTextures), bufferObject_(bufferObject), useInstancing_(useInstancing)
{
    TransluencyType_t transluencyType = material_->GetTransluencyType();
    key_ |= ((uint64_t)layer_) << LAYER_SHIFT;
    key_ |= ((uint64_t)(distanceFromCamera  & DISTANCE_TRUNCATION) << ((transluencyType == TRANSLUENCY_TYPE_OPAQUE) ? 0 : DEPTH_SHIFT));
    key_ |= ((uint64_t)ConstructMaterialId() << ((transluencyType == TRANSLUENCY_TYPE_OPAQUE) ? MATERIAL_SHIFT : 0));
}

RenderQueueItem::~RenderQueueItem() {
}

uint32_t RenderQueueItem::ConstructMaterialId() const {
    uint16_t shaderId = material_->GetShader()->GetId();
    return shaderId;
}

bool RenderQueueItemKeyComparator::operator() (size_t lhs, size_t rhs) const {
    // For opaque objects, we sort by material first to minimize state changes. Otherwise,
    // we sort by depth first, and back-to-front.
    const RenderQueueItem& x = items_[lhs];
    const RenderQueueItem& y = items_[rhs];

    return x.key_ < y.key_;
    /*
    if (x.layer_ > y.layer_) {
        return false;
    } else if (x.layer_ == y.layer_) {
        if (x.transluencyType_ > y.transluencyType_) {
            return false;
        } else if (x.transluencyType_ == y.transluencyType_) {
            TransluencyType_t transluency = x.transluencyType_;

            switch (transluency) {
                case TRANSLUENCY_TYPE_OPAQUE:
                    if (x.materialId_ > y.materialId_) {
                        return false;
                    } else if (x.materialId_ == y.materialId_) {
                        if (x.distanceFromCamera_ >= y.distanceFromCamera_) {
                            return false;
                        }
                    }
                    break;
                case TRANSLUENCY_TYPE_ADDITIVE:
                case TRANSLUENCY_TYPE_SUBTRACTIVE:
                    if (x.distanceFromCamera_ < y.distanceFromCamera_) {
                        return false;
                    } else if (x.distanceFromCamera_ == y.distanceFromCamera_) {
                        if (x.materialId_ >= y.materialId_) {
                            return false;
                        }
                    }
                    break;
            }
        }
    }

    return true;
    */
}

}
