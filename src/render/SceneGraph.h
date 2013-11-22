#ifndef SKETCH_3D_SCENE_GRAPH_H
#define SKETCH_3D_SCENE_GRAPH_H

#include <map>
#include <string>
#include <vector>
using namespace std;

namespace Sketch3D
{
// Forward class declaration
class Geometry;
class Node;
class RenderState;
class RenderSystem;
class Spatial;

/**
 * @class SceneGraph
 * This class represents a SceneGraph used to render the nodes in it. On
 * construction, it will create a Spatial node that represents the Identity
 * matrix and a RenderState node that represents the basic rendering attributes.
 * Their name are "SpatialRoot" and "RenderStateRoot" respectively. The Spatial
 * node is the actual root.
 */
class SceneGraph
{
    public:
        /**
         * Default constructor
         */
                            SceneGraph();

        /**
         * Destructor
         */
                           ~SceneGraph();

        /**
         * Draws the whole scene graph by starting from the root node
         * @param renderSystem The RenderSystem to use to render the graph
         */
        void                Draw(RenderSystem* renderSystem) const;

        /**
         * Create a render state node. It will attach it by default to the
         * spatial root.
         * @param name The name to give to the node
         * @return A pointer to the node. If the node already exists by this
         * name it will return NULL
         */
        RenderState*        CreateRenderStateNode(const string& name);    

        /**
         * Create a spatial node. It will attach it by default to the spatial
         * root.
         * @param name The name to give to the node
         * @return A pointer to the node. If the node already exists by this
         * name it will return NULL
         */
        Spatial*            CreateSpatialNode(const string& name);

        /**
         * Create a geometry node. It will attach it by default to the spatial
         * root.
         * @param name The name to give to the node
         * @return A pointer to the node. If the node already exists by this
         * name it will return NULL
         */
        Geometry*           CreateGeometryNode(const string& name);

        /**
         * Delete the specified node. If the node has children, they will all be
         * destroyed. If the node isn't found, nothing will happen.
         * @param name The name of the node to delete
         */
        void                DeleteNode(const string& name);

        /**
         * Retrieves the root node.
         */
        Node*               GetRoot() const;

        /**
         * Retrieves the Spatial root. This is the first spatial node
         * encountered in the graph.
         */
        Spatial*            GetSpatialRoot() const;

        /**
         * Retrieves a node from the graph by using its name
         * @param name The name of the node
         * @return A pointer to the node. NULL if the node does not exist
         */
        Node*               GetNode(const string& name) const;

        /**
         * Retrieves a Spatial node from the graph by using its name. The
         * required node has to be a spatial node.
         * @param name The name of the Spatial node to get.
         * @return A pointer to the node. NULL if it doesn't exists.
         */
        Spatial*            GetSpatialNode(const string& name) const;

        /**
         * Retrieves a RenderState node from the graph by using its name. The
         * required node has to be a RenderState node.
         * @param name The name of the RenderState node to get.
         * @return A pointer to the node. NULL if it doesn't exists.
         */
        RenderState*        GetRenderStateNode(const string& name) const;

        /**
         * Retrieves a Geometry node from the graph by using its name. The
         * required node has to be a Geometry node.
         * @param name The name of the Geometry node to get.
         * @return A pointer to the node. NULL if it doesn't exists.
         */
        Geometry*           GetGeometryNode(const string& name) const;

        /**
         * Clears the scene graph and free all memory. This removes the root as
         * well.
         */
        void                Clear();

        /**
         * Clears the scene graph and free all memory but keeps the roots.
         */
        void                ClearKeepRoots();
        
    private:
        Node*               root_;          /**< The root node of the graph */
        Spatial*            spatialRoot_;   /**< The spatial root node */
        map<string, Node*>  nodes_;         /**< All the nodes */
};
}

#endif
