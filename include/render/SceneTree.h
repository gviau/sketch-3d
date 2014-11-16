#ifndef SKETCH_3D_SCENE_TREE_H
#define SKETCH_3D_SCENE_TREE_H

#include "render/Node.h"
#include "render/RenderQueue.h"

#include <string>
using namespace std;

namespace Sketch3D {

/**
 * @class SceneTree
 * The SceneTree class is a CompositeNode that has additional functionnality
 * to render a whole scene. The actual rendering process isn't done by this
 * class; its responsability is to accumulate the geometry and transformation
 * of all its children node and send it for rendering.
 *
 * The name of this node is automatically set to "root" and it has no parent.
 */
class SceneTree {
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
		 * Render all the nodes in the tree. Note that this call doesn't
		 * immediatly render the nodes on the screen, but it prepares the
		 * rendering process.
		 */
		void		        Render();

        /**
         * Add a node to the scene tree. This will add the node directly to the root node
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
        RenderQueue         renderQueue_;   /**< The render queue used for drawing */
        Node                root_;          /**< The root node of the scene tree */
};

}

#endif