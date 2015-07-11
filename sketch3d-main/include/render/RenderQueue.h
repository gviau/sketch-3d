//#ifndef SKETCH_3D_RENDER_QUEUE_H
//#define SKETCH_3D_RENDER_QUEUE_H
//
//#include "render/RenderQueueItem.h"
//
//#include "system/Platform.h"
//
//#include <vector>
//using namespace std;
//
//namespace Sketch3D {
//
//// Forward declaration
//class Node;
//
//**
// * @class RenderQueue
// * Implements a render queue. Each item in the queue is sorted to allow faster drawing.
// * The render queue will also try to batch geometry data if they share the same material and
// * that they are both the same type of vertex buffers (static or dynamic)
// */
//class SKETCH_3D_API RenderQueue {
//    public:
//        /**
//         * Constructor
//         */
//                                    RenderQueue();
//
//        /**
//         * Adds a node to the queue. This will actually add all of SurfaceTriangles_t object of the mesh contained by the node, along with its model
//         * matrix.
//         * @param node The node to render
//         * @param layer On which layer are we drawing everything. This option might change render state, such as depth testing
//         */
//        void                        AddNode(Node* node, Layer_t layer=LAYER_GAME);
//
//        /**
//         * Renders the content of the queue. This invalidates the queue by removing all items in the queue.
//         */
//        void                        Render();
//
//        /**
//         * Checks if the render queue is empty or not
//         */
//        bool                        IsEmpty() const;
//
//    private:
//         vector<RenderQueueItem>    items_;         /**< The items in the queue */
//         vector<size_t>             itemsIndex_;    /**< This list is the list that get actually sorted. We can then use temporal coherence to have less things to sort on subsequent frames */
//};
//
//}
//
//#endif