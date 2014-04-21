#ifndef SKETCH_3D_SCENE_TREE_H
#define SKETCH_3D_SCENE_TREE_H

#include "CompositeNode.h"

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
class SceneTree : public CompositeNode {
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
		virtual void		render() const;

	private:
};

}

#endif