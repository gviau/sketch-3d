#ifndef SKETCH_3D_SCENE_TREE_H
#define SKETCH_3D_SCENE_TREE_H

#include "render/Node.h"

#include "system/Platform.h"

#include <string>
using namespace std;

namespace Sketch3D {
// Forward struct declaration
struct FrustumPlanes_t;

// Forward class declaration
class RenderQueue;

/**
 * @class SceneTree
 * The SceneTree class is a CompositeNode that has additional functionnality
 * to render a whole scene. The actual rendering process isn't done by this
 * class; its responsability is to accumulate the geometry and transformation
 * of all its children node and send it for rendering.
 */
class SKETCH_3D_API SceneTree {
	public:
		/**
		 * Constructor
		 */
							SceneTree();

		/**
		 * Destructor - free all children node
		 */
		virtual			   ~SceneTree();
			
		/**
         * Populate the render queue with nodes. This will cull nodes that aren't in the view frustum and
         * prepare the data for the actual rendering process
         * @param frustumPlanes The 6 view frustum planes to cull objects that are not visible by the camera
         * @param useFrustumCulling If set to true, the frustum planes will be used to cull objects
		 * @param opaqueRenderQueue The render queue to populate with opaque objects
         * @param transparentRenderQueue The render queue to populate with transparent objects
		 */
		void		        Render(const FrustumPlanes_t& frustumPlanes, bool useFrustumCulling, RenderQueue& opaqueRenderQueue,
                                   RenderQueue& transparentRenderQueue);

        /**
         * Add a node to the scene tree. This will add the node directly to the root node. It is the responsability of the  caller
         * to free the memory used by the node. The SceneTree will free no memory for the nodes
         * @param node A pointer to the node to add to the scene tree
         * @return true if the node could be added, false if the node was already present
         */
        bool                AddNode(Node* node);

        /**
         * Get a node from the scene tree by its name.
         * @param name The name of the node to get
         * @return A pointer to the node if it is in the scene tree, nullptr otherwise
         */
        Node*               GetNodeByName(const string& name) const;

        /**
         * Remove a node.
         * @param node A pointer to the node to remove
         * @return true if the node was succesfully removed, false if it couldn't be found
         */
        bool                RemoveNode(const Node* const node);

        /**
         * Remove a node by its name.
         * @param name The name of the node to remove
         * @return true if the node was succesfully removed, false if it couldn't be found
         */
        bool                RemoveNodeByName(const string& name);

    private:
        Node                root_;          /**< The root node of the scene tree */
};

}

#endif