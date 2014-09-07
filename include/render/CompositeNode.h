#ifndef SKETCH_3D_COMPOSITE_NODE_H
#define SKETCH_3D_COMPOSITE_NODE_H

#include "render/Node.h"

#include <map>
using namespace std;

namespace Sketch3D {

/**
 * @class CompositeNode
 * The CompositeNode is a node that can have one or more child node. When
 * applying functions on this node, it will call the same function on all its
 * children to propagate the effect.
 */
class CompositeNode : public Node {
	public:
		/**
		 * Constructor. The name of the node is generated automatically.
		 * @param parent This node parent. NULL by default
		 */
						CompositeNode(Node* parent=NULL);

		/**
		 * Constructor
		 * @param name The name of this node
		 * @param parent The parent of this node. NULL by default
		 */
						CompositeNode(const string& name, Node* parent=NULL);

		/**
		 * Constructor. The name of the node is generated automatically.
		 * @param position The position of the node
		 * @param scale The scaling factor of the node
		 * @param orientation The orientation of the node
		 * @param parent The parent of this node. NULL by default
		 */
						CompositeNode(const Vector3& position, const Vector3& scale,
									  const Quaternion& orientation, Node* parent=NULL);

		/**
		 * Constructor
		 * @param name The name of this node
		 * @param position The position of the node
		 * @param scale The scaling factor of the node
		 * @param orientation The orientation of the node
		 * @param parent The parent of this node. NULL by default
		 */
							CompositeNode(const string& name, const Vector3& position,
										  const Vector3& scale, const Quaternion& orientation,
										  Node* parent=NULL);

		/**
		 * Destructor - free all children nodes
		 */
		virtual			   ~CompositeNode();

		/**
		 * Render this node and its children. Note that this call doesn't
		 * immediatly render the nodes on the screen, but it prepares the
		 * rendering process.
         * @param renderQueue The render queue to use for drawing
		 */
		virtual void		Render(RenderQueue& renderQueue ) const;

		/**
		 * Add a node to this node's childrens. The name of the node must not already
		 * exist in this node's list
		 * @param node The node to add to the list
		 * @return true if the node could be added, false otherwise.
		 */
		bool				AddChildren(Node* node);

		/**
		 * Remove a node from this node
		 * @param name The name of the node to remove
		 * @return true if the node was found. False otherwise
		 */
		bool				RemoveChildren(const string& name);

	protected:
		map<string, Node*>	children_;	/**< The children of this node. The key is the name of the node */
};

}

#endif