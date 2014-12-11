#ifndef SKETCH_3D_RENDER_QUEUE_ITEM_H
#define SKETCH_3D_RENDER_QUEUE_ITEM_H

#include "render/Material.h"

#include <map>
#include <stdint.h>
#include <string>
#include <utility>
using namespace std;

namespace Sketch3D {

// Forward declaration
class Node;
class Vector3;
class Vector4;
class Matrix3x3;
class Matrix4x4;

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
class RenderQueueItem {
    friend class RenderQueue;
    friend class RenderQueueItemKeyComparator;

    public:
        /**
         * Constructor.
         * Creates the render queue from a node
         * @param node The node that contains the mesh and material to draw
         * @param distanceFromCamera The distance that the mesh is from the camera
         * @param layer On which layer are we drawing everything. This option might change render state, such as depth testing
         */
                            RenderQueueItem(const Node* node, float distanceFromCamera, Layer_t layer=LAYER_GAME);

        /**
         * Destructor. Frees the uniform map
         */
                           ~RenderQueueItem();

    private:
        // Key stuff
        Layer_t             layer_;
        TransluencyType_t   transluencyType_;
        float               distanceFromCamera_;
        int                 materialId_;

        const Node*         node_;  /**< The node to draw */

        /**
         * Construct the 30 bits material id from the material properties
         */
        uint32_t            ConstructMaterialId() const;
};


class RenderQueueItemKeyComparator {
    public:
        bool operator() (const RenderQueueItem* x, const RenderQueueItem* y) const;
};

}

#endif
