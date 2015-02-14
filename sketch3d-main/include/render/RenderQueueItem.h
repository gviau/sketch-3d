#ifndef SKETCH_3D_RENDER_QUEUE_ITEM_H
#define SKETCH_3D_RENDER_QUEUE_ITEM_H

#include "render/Material.h"

#include "math/Matrix4x4.h"

#include "system/Platform.h"

#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>
using namespace std;

namespace Sketch3D {

// Forward declaration
class BufferObject;
class Node;
class Vector3;
class Vector4;
class Matrix3x3;
class Texture2D;

enum Layer_t {
    LAYER_GAME,
    LAYER_FULLSCREEN,
    LAYER_HUD
};

/**
 * @class RenderQueueItem
 * Creates a render queue item which is used to sort draw commands. The render queue item is a pair of key and value,
 * where the key is used to sort and the value is the actual drawing data.
 *
 * The key is composed of the following components
 *
 *  - Fullscreen layer :
 *      --> 00 = game layer (normal drawing);
 *      --> 01 = Fullscreen effect;
 *      --> 10 = HUD.
 *  - Transulency type :
 *      --> 00 = Opaque geometry;
 *      --> 01 = Additive transparency;
 *      --> 10 = Subtractive transparency.
 *  - Depth : The depth value of the object.
 *            When the transluency type is opaque, this is used as front-to-back sorting, otherwise, this is back-to-front sorting
 *  - Material id : The id of the material used.
 *
 * NOTE: if the transluency type is opaque, the Material Id and Depth value are swapped together, so that opaque objects are sorted
 * through material ids before depth. Otherwise, we sort via depth.
 */
class SKETCH_3D_API RenderQueueItem {
    friend class RenderQueue;
    friend class RenderQueueItemKeyComparator;

    public:
        /**
         * Constructor.
         * Creates a render queue item
         * @param modelMatrix The modelMatrix to position the sub-mesh
         * @param material The material to use to draw the sub-mesh
         * @param textures The textures to use on this sub-mesh
         * @param numTextures The number of textures to use on this sub-mesh
         * @param bufferObject The buffer object representing the actual sub-mesh to draw
         * @param useInstancing If set to true, the bufferObject will be used in instanced rendering
         * @param distanceFromCamera The distance that the mesh is from the camera, normalized on the distance between the near and far plane of the camera.
         * A distance of 0 means on the near plane and a distance corresponding to the maximum value of a unsigned 32 btis word means on the far plane.
         * @param layer On which layer are we drawing everything. This option might change render state, such as depth testing
         */
                            RenderQueueItem(shared_ptr<Matrix4x4> modelMatrix, Material* material, Texture2D** textures,
                                            size_t numTextures, BufferObject* bufferObject, bool useInstancing,
                                            uint32_t distanceFromCamera, Layer_t layer=LAYER_GAME);

        /**
         * Destructor. Frees the uniform map
         */
                           ~RenderQueueItem();

    private:
        // Key stuff
        uint64_t            key_;
        Layer_t             layer_;
        TransluencyType_t   transluencyType_;
        uint32_t            distanceFromCamera_;
        int                 materialId_;

        shared_ptr<Matrix4x4>   modelMatrix_;       /**< The model matrix to use to position the sub-mesh */
        Material*           material_;          /**< The material to use to draw the sub-mesh */
        Texture2D**         textures_;          /**< The textures to use on this sub-mesh */
        size_t              numTextures_;       /**< The number of textures to use on this sub-mesh */
        BufferObject*       bufferObject_;      /**< The buffer object representing the actual sub-mesh to draw */
        bool                useInstancing_;     /**< Determine if we draw multiple instances of the buffer object */

        /**
         * Construct the 30 bits material id from the material properties
         */
        uint32_t            ConstructMaterialId() const;
};


class SKETCH_3D_API RenderQueueItemKeyComparator {
    public:
        RenderQueueItemKeyComparator(const vector<RenderQueueItem>& items) : items_(items) {
        }

        bool operator() (size_t lhs, size_t rhs) const;

    private:
        const vector<RenderQueueItem>& items_;
};

}

#endif
