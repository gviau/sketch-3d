#ifndef SKETCH_3D_NODE_H
#define SKETCH_3D_NODE_H

#include "math/Matrix4x4.h"
#include "math/Quaternion.h"
#include "math/Vector3.h"

#include <string>
using namespace std;

namespace Sketch3D {

/**
 * @class Node
 * This class provides the base functionnality of a node the SceneTree
 */
class Node {
	public:
		/**
		 * Constructor. The name of the node is generated automatically.
		 * @param parent The parent of this node. NULL by default.
		 */
							Node(Node* parent=NULL);

		/**
		 * Constructor
		 * @param name The name of this node
		 * @param parent The parent of this node. NULL by default
		 */
							Node(const string& name, Node* parent=NULL);

		/**
		 * Constructor. The name of the node is generated automatically.
		 * @param position The position of this node
		 * @param scale The scale of this node
		 * @param orientation The orientation of this node
		 * @param parent The parent of this node. NULL by default
		 */
							Node(const Vector3& position, const Vector3& scale,
								 const Quaternion& orientation, Node* parent=NULL);

		/**
		 * Constructor
		 * @param name The name of this node
		 * @param position The position of this node
		 * @param scale The scale of this node
		 * @param orientation The orientation of this node
		 * @param parent The parent of this node. NULL by default
		 */
							Node(const string& name, const Vector3& position,
								 const Vector3& scale, const Quaternion& orientation,
								 Node* parent=NULL);

		/**
		 * Destructor
		 */
		virtual			   ~Node();

		/**
		 * Render the node. Note that this call doesn't immediatly render the
		 * node on the screen, but it prepares the rendering process.
		 */
		virtual void		Render() const;

		/**
		 * Translate the node by a certain amount. This amount is added to the
		 * position that the node currently have
		 * @param translation The translation to add
		 */
		void				Translate(const Vector3& translation);

		/**
		 * Scale the node by a certain amount. This amount is multiplied by
		 * the scale that the node currently have
		 * @param scale The scaling factor to multiply
		 */
		void				Scale(const Vector3& scale);

		/**
		 * Pitch the node by a certain amount. This amount is added to the
		 * current orientation that the node currently have
		 * @param angle The angle to pitch the node
		 */
		void				Pitch(float angle);

		/**
		 * Yaw the node by a certain amount. This amount is added to the
		 * current orientation that the node currently have
		 * @param angle The angle to yaw the node
		 */
		void				Yaw(float angle);

		/**
		 * Roll the node by a certain amount. This amount is added to the
		 * current orientation that the node currently have
		 * @param angle The angle to roll the node
		 */
		void				Roll(float angle);

		/**
		 * Rotate the node arount a specified axis. The amount of rotation is
		 * added to the current orientation that the node currently have
		 * @param angle The angle to rotate the node
		 * @param axis The axis around which the node should be rotated
		 */
		void				RotateAroundAxis(float angle, const Vector3& axis);

		void				SetParent(Node* parent);
		void				SetPosition(const Vector3& position);
		void				SetScale(const Vector3& scale);
		void				SetOrientation(const Quaternion& orientation);

		const string&		GetName() const;
		Node*				GetParent() const;
		const Vector3&		GetPosition() const;
		const Vector3&		GetScale() const;
		const Quaternion&	GetOrientation() const;

	protected:
		static long long	nextNameIndex_;	/**< The next available number for the automatic name */

		string				name_;		/**< The name of this node */
		Node*				parent_;	/**< The parent of this node */

		Vector3				position_;	/**< The position of this node */
		Vector3				scale_;		/**< The scale factor of this node */
		Quaternion			orientation_;	/**< The orientation of this node */

		Matrix4x4			cachedTransformation_;	/**< The cached matrix transformation */
};

}

#endif