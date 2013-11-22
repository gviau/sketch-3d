#ifndef SKETCH_3D_GEOMETRY_H
#define SKETCH_3D_GEOMETRY_H

#include <vector>
using namespace std;

#include "Node.h"

namespace Sketch3D
{
// Forward class declaration
class Matrix4x4;
class RenderSystem;
class Spatial;
class Vector3;

/**
 * @class Geometry
 * Represents a node that contains information useful to render a model, such
 * as its vertices and normals.
 */
class Geometry : public Node
{
    public:
        /**
         * Constructor
         * @param name The name to give this node
         */
                            Geometry(const string& name);

        virtual void        Draw(RenderSystem* renderSystem) const;

        /**
         * Translates the geometry. This will go upwards the scene graph to
         * update the first Spatial node it finds if it only has one child. If
         * the spatial node has more than one child, it will create a new
         * spatial node, make it this node's parent and update the graph.
         * @param x The amount on the x axis to translate
         * @param y The amount on the y axis to translate
         * @param z The amount on the z axis to translate
         */
        void                Translate(float x, float y, float z);

        /**
         * Translates the geometry. This will go upwards the scene graph to
         * update the first Spatial node it finds if it only has one child. If
         * the spatial node has more than one child, it will create a new
         * spatial node, make it this node's parent and update the graph.
         * @param translation A vector representing the translation
         */
        void                Translate(const Vector3& translation);

        /**
         * Rotates the geometry around the X axis. This will go upwards the scene
         * graph to update the first Spatial node it finds if it only has one
         * child. If the spatial node has more than one child, it will create a
         * new spatial node, make it this node's parent and update the graph.
         * @param angle The angle in radians to rotate the geometry
         */
        void                RotateX(float angle);

        /**
         * Rotates the geometry around the Y axis. This will go upwards the scene
         * graph to update the first Spatial node it finds if it only has one
         * child. If the spatial node has more than one child, it will create a
         * new spatial node, make it this node's parent and update the graph.
         * @param angle The angle in radians to rotate the geometry
         */
        void                RotateY(float angle);

        /**
         * Rotates the geometry around the Z axis. This will go upwards the scene
         * graph to update the first Spatial node it finds if it only has one
         * child. If the spatial node has more than one child, it will create a
         * new spatial node, make it this node's parent and update the graph.
         * @param angle The angle in radians to rotate the geometry
         */
        void                RotateZ(float angle);

        /**
         * Scales the geometry. This will go upwards the scene graph to
         * update the first Spatial node it finds if it only has one child. If
         * the spatial node has more than one child, it will create a new
         * spatial node, make it this node's parent and update the graph.
         * @param x The amount to scale on the x axis
         * @param y The amount to scale on the y axis
         * @param z The amount to scale on the z axis
         */
        void                Scale(float x, float y, float z);

        /**
         * Scales the geometry. This will go upwards the scene graph to
         * update the first Spatial node it finds if it only has one child. If
         * the spatial node has more than one child, it will create a new
         * spatial node, make it this node's parent and update the graph.
         * @param scale A vector that represents the amount to scale
         */
        void                Scale(const Vector3& scale);

        virtual nodeType_t  GetType() const;
    private:
        vector<Vector3>     vertices_;  /**< List of vertices that compose the model. */
        vector<Vector3>     normals_;   /**< Vertices' normals. */
        vector<int>         indices_;   /**< Vertices' indices. */

        /**
         * Return the Spatial parent of this node. If the immediate parent is
         * not a Spatial node, we create one and restructure the graph.
         */
        Spatial*            GetSpatialParent();
};
}

#endif
