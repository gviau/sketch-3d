#ifndef SKETCH_3D_NODE_H
#define SKETCH_3D_NODE_H

#include "math/Matrix4x4.h"
#include "math/Quaternion.h"
#include "math/Vector3.h"

#include "system/Platform.h"

#include <map>
#include <string>
using namespace std;

namespace Sketch3D {
// Forward struct declaration
struct FrustumPlanes_t;

// Forward class declaration
class Material;
class Mesh;
class RenderQueue;

/**
 * @class Node
 * This class provides the base functionnality of a node the SceneTree
 */
class SKETCH_3D_API Node {
    friend class SceneTree;

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
         * Copy constructor
         * @param src The node to copy
         */
                            Node(const Node& src);

		/**
		 * Destructor
		 */
		virtual			   ~Node();

        /**
         * Renders the node on the screen
         */
        void                Render();

		/**
		 * Add a node to this node's childrens. The name of the node must not already
		 * exist in this node's list
		 * @param node The node to add to the list
		 * @return true if the node could be added, false otherwise.
		 */
		bool				AddChildren(Node* node);

        /**
         * Returns the children node with the specified name
         * @param name The name of the node to get
         * @return A pointer to the node or nullptr if it couldn't be found
         */
        Node*               GetNodeByName(const string& name) const;

        /**
         * Remove a children node from this node
         * @param node The node to remove
         * @return true if the node was found. False otherwise
         */
        bool                RemoveChildren(const Node* const node);

		/**
		 * Remove a children node from this node by its name
		 * @param name The name of the node to remove
		 * @return true if the node was found. False otherwise
		 */
		bool				RemoveChildrenByName(const string& name);

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

        /** 
         * Construct the model matrix for this node
         */
        Matrix4x4           ConstructModelMatrix();

		void				SetParent(Node* parent);
		void				SetPosition(const Vector3& position);
		void				SetScale(const Vector3& scale);
		void				SetOrientation(const Quaternion& orientation);
		void				SetMesh(Mesh* mesh);
		void				SetMaterial(Material* material);
        void                SetInstancing(bool useInstancing);

		const string&		GetName() const;
		Node*				GetParent() const;
		const Vector3&		GetPosition() const;
		const Vector3&		GetScale() const;
		const Quaternion&	GetOrientation() const;
		Mesh*				GetMesh() const;
		Material*			GetMaterial() const;
        bool                UseInstancing() const;

	private:
		static long long	nextNameIndex_;	/**< The next available number for the automatic name */

		string				name_;		/**< The name of this node */
		Node*				parent_;	/**< The parent of this node */
        map<string, Node*>	children_;	/**< The children of this node. The key is the name of the node */

		Vector3				position_;	/**< The position of this node */
		Vector3				scale_;		/**< The scale factor of this node */
		Quaternion			orientation_;	/**< The orientation of this node */

        Matrix4x4*          parentTransformation_;  /**< Transformation matrix from the parent */
		Matrix4x4			cachedTransformation_;	/**< The cached matrix transformation */
        bool                needTransformationUpdate_;

		Mesh*				mesh_; /**< Geometric represention of the node */
		Material*			material_; /**< The material of the node. It will be applied to the mesh, if any */
        
        bool                useInstancing_; /**< If set to true, the node will use instanced rendering */

		/**
		 * This function sends the data required for the rendering.
         * @param frustumPlanes The 6 view frustum planes to cull nodes
         * @param useFrustumCulling If set to true, the frustum planes will be used to cull this node
         * @param opaqueRenderQueue The render queue to use for drawing opaque objects
         * @param transparentRenderQueue The render queue to use for drawing transparent objects
		 */
		void                Render(const FrustumPlanes_t& frustumPlanes, bool useFrustumCulling, RenderQueue& opaqueRenderQueue,
                                   RenderQueue& transparentRenderQueue);
};

}

#endif