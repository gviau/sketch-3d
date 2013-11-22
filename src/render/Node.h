#ifndef SKETCH_3D_NODE_H
#define SKETCH_3D_NODE_H

#include <string>
#include <list>
using namespace std;

namespace Sketch3D
{
// Forward class declaration
class RenderState;
class RenderSystem;

/**
 * @class Node
 * This class represents a Node in the scene graph. It can be either a geometry,
 * which is an object that will be rendered on the screen, or a spatial node,
 * which contains a transformation matrix that will transform all its children.
 */
class Node
{
    public:
        /**
         * @enum nodeType_t
         * Defines the type of the node
         */
        enum nodeType_t {
            SPATIAL,
            RENDERSTATE,
            GEOMETRY
        };

        /**
         * Constructor.
         * @param name A reference to the name of the node.
         */
                                Node(const string& name);

        /**
         * Destructor
         */
        virtual                ~Node();

        /**
         * Draw the node with the render system
         * @param renderSystem The RenderSystem to use to draw the node
         */
        virtual void            Draw(RenderSystem* renderSystem) const = 0;

        /**
         * Propagate the render state of from the root of the graph to all nodes
         * @param newState A reference to a RenderState object representing the
         * new render state to use for this node
         */
        void                    PropagateRenderStateFromRoot(const RenderState& newState);

        /**
         * Updates the render state of this node
         * @param newState A reference to a RenderState object representing the
         * new render state to use for this node
         */
        virtual void            UpdateRenderState(const RenderState& newState);

        /**
         * Adds a child node to this one
         * @param node The new node to add
         */
        void                    AddChild(Node* node);

        /**
         * Removes the child node from this one
         * @param node The node to remove
         */
        void                    RemoveChild(Node* node);

        /**
         * Removes the child node from this one
         * @param index The index at which the node should be removed
         */
        void                    RemoveChildByIndex(size_t index);

        /**
         * Removes the child node from this one
         * @param name The name of the node to remove
         */
        void                    RemoveChildByName(const string& name);

        /**
         * Gets the list of all children. This is the read-only version
         * @return A vector to the nodes
         */
        const list<Node*>&      GetChildren() const;

        /**
         * Gets the list of all children. This is the mutable version
         * @return A vector to the nodes
         */
        list<Node*>&            GetChildren();

        /**
         * Gets a single node at the specified index.
         * @param index The index in the list
         * @return A pointer to the node. NULL if the index is invalid
         */
        Node*                   GetChildByIndex(size_t index) const;

        /**
         * Gets a single node from its name
         * @param name The name of the node
         * @return A pointer to the node. NULL if the node wasn't found
         */
        Node*                   GetChildByName(const string& name) const;

        /**
         * Replaces a node with another one by its index
         * @param index The index in the list
         * @param node The node to replace the old one with
         * @return false if the index is invalid
         */
        bool                    ReplaceNodeByIndex(size_t index, Node* node);

        /**
         * Replaces a node with another one by its name
         * @param name The name of the node to replace
         * @param node The node to replace the old one with
         * @return false if the node wasn't found
         */
        bool                    ReplaceNodeByName(const string& name,
                                                  Node* node);

        /**
         * Remove all children from this node
         */
        void                    RemoveAllChildren();

        void                    SetParent(Node* parent);
        Node*                   GetParent() const;
        const string&           GetName() const;

        /**
         * Returns the type of the node
         */
        virtual Node::nodeType_t GetType() const = 0;
    protected:
        Node*                   parent_;    /**< The parent of this node */
        list<Node*>             children_;  /**< List of children of this node */
        string                  name_;      /**< Reference to the name of the node */

        // FUNCTORS
        /**
         * @class HasSameName
         * Functor used to determined if two nodes have the same name
         * Used for linked list algorithms
         */
        class HasSameName
        {
            public:
                                HasSameName(const string& name) : name_(name) {}

                bool            operator()(Node* node) { return node->name_ == name_; }
            private:
                const string&   name_;
        };
};
}

#endif
