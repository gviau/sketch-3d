#ifndef SKETCH_3D_SPATIAL_H
#define SKETCH_3D_SPATIAL_H

#include "Matrix4x4.h"
#include "Node.h"

namespace Sketch3D
{

/**
 * @class Spatial
 * Represents a node that contains a transformation matrix. All of its children
 * will be transformed by it.
 */
class Spatial : public Node
{
    public:
        /**
         * Constructor.
         * @param name A reference to the name of the node
         */
                        Spatial(const string& name);

        virtual void        Draw(RenderSystem* renderSystem) const;

        /**
         * Sets a new transformation matrix for this node
         * @param transform The new transformation matrix
         */
        void                SetTransformation(const Matrix4x4& transform);

        /**
         * Returns the transformation matrix
         */
        Matrix4x4           GetTransformation() const;

        virtual nodeType_t  GetType() const;
    protected:
        Matrix4x4           transformMatrix_;   /**< The transformation matrix for this node */
};
}

#endif
