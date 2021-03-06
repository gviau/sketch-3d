#ifndef SKETCH_3D_SCENE_TREE_H
#define SKETCH_3D_SCENE_TREE_H

#include "render/Node.h"

#include "system/Platform.h"

#include <map>
#include <string>
#include <vector>
using namespace std;

namespace Sketch3D {
// Forward struct declaration
struct FrustumPlanes_t;
struct SurfaceTriangles_t;

// Forward class declaration
class BufferObject;
class RenderQueue;
class Shader;
class Texture2D;

/**
 * @class SceneTree
 * The SceneTree class is a CompositeNode that has additional functionnality
 * to render a whole scene. The actual rendering process isn't done by this
 * class; its responsability is to accumulate the geometry and transformation
 * of all its children node and send it for rendering.
 */
class SKETCH_3D_API SceneTree {
    typedef pair<size_t, Texture2D**>                   TexturesPair_t;
    typedef pair<TexturesPair_t, vector<BufferObject*>> TexturesBuffersPair_t;
    typedef map<size_t, TexturesBuffersPair_t>          TexturesToBuffersMap_t;
    typedef map<Shader*, TexturesToBuffersMap_t>        StaticBatches_t;

	public:
		/**
		 * Constructor
		 */
							        SceneTree();

		/**
		 * Destructor - free all children node
		 */
		virtual			           ~SceneTree();
			
		/**
         * Populate the render queue with nodes. This will cull nodes that aren't in the view frustum and
         * prepare the data for the actual rendering process
         * @param frustumPlanes The 6 view frustum planes to cull objects that are not visible by the camera
         * @param useFrustumCulling If set to true, the frustum planes will be used to cull objects
		 * @param opaqueRenderQueue The render queue to populate with opaque objects
         * @param transparentRenderQueue The render queue to populate with transparent objects
		 */
		void		                Render(const FrustumPlanes_t& frustumPlanes, bool useFrustumCulling, RenderQueue& opaqueRenderQueue,
                                           RenderQueue& transparentRenderQueue);

        /**
         * Render the static batches
         */
        void                        RenderStaticBatches() const;

        /**
         * Add a node to the scene tree. This will add the node directly to the root node. It is the responsability of the  caller
         * to free the memory used by the node. The SceneTree will free no memory for the nodes
         * @param node A pointer to the node to add to the scene tree
         * @return true if the node could be added, false if the node was already present
         */
        bool                        AddNode(Node* node);

        /**
         * Get a node from the scene tree by its name.
         * @param name The name of the node to get
         * @return A pointer to the node if it is in the scene tree, nullptr otherwise
         */
        Node*                       GetNodeByName(const string& name) const;

        /**
         * Remove a node.
         * @param node A pointer to the node to remove
         * @return true if the node was succesfully removed, false if it couldn't be found
         */
        bool                        RemoveNode(const Node* const node);

        /**
         * Remove a node by its name.
         * @param name The name of the node to remove
         * @return true if the node was succesfully removed, false if it couldn't be found
         */
        bool                        RemoveNodeByName(const string& name);

        /**
         * Iterate over all nodes in the scene tree to perform static batching over all nodes
         * marked as static
         */
        void                        PerformStaticBatching();

    private:
        Node                        root_;          /**< The root node of the scene tree */
        StaticBatches_t             staticBatches_; /**< List of batches to draw */
        vector<SurfaceTriangles_t*> preTransformedSurfaces_;    /**< List of pretransformed surfaces used in static batches */
};

}

#endif