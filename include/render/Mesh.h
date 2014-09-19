#ifndef SKETCH_3D_MESH_H
#define SKETCH_3D_MESH_H

#include "math/Vector2.h"
#include "math/Vector3.h"

#include "render/ResourceManager.h"

#include <string>
#include <vector>
using namespace std;

namespace Sketch3D {

/**
 * @struct SurfaceTriangles_t
 * Structure containing the information about a surface to draw regarding the vertices
 */

/**
 * @enum MeshType_t
 * The type of mesh that will be rendered, static or dynamic
 */
enum MeshType_t {
    MESH_TYPE_STATIC,
    MESH_TYPE_DYNAMIC,
};

/**
 * @class Mesh
 * This class holds the representation of a 3D model. It is reponsible to send
 * the needed data to the underlying rendering system.
 */
class Mesh {
	public:
        /**
         * Constructor. Initialize everything to 0
         */
                                Mesh();

		/**
		 * Constructor. Read a mesh from a file and load the data in memory
		 * @param filename The name of the file containing the model data
         * @param meshType The type of mesh that will be rendered, static or dynamic
		 */
                                Mesh(const string& filename, MeshType_t meshType=MESH_TYPE_STATIC);

        /**
         * Destructor - release the buffer objects
         */
                               ~Mesh();

        /**
         * Load the model from a file
         * @param filename
         */
        void                    Load(const string& filename);

        /**
         * Initialize the mesh with geometry data
         * @param meshType The type of mesh that will be rendered, static or dynamic
         */
        void                    Initialize(MeshType_t meshType=MESH_TYPE_STATIC);

        /**
         * Add vertices to the mesh
         * @param vertices The list of vertices to add
         */
        void                    SetVertices(const vector<Vector3>& vertices);

        /**
         * Add normals to the mesh
         * @param normals The list of normals to add
         */
        void                    SetNormals(const vector<Vector3>& normals);

        /**
         * Add texture coords to the mesh
         * @param texCoords The list of texture coords to add
         */
        void                    SetTextureCoords(const vector<Vector2>& texCoords);

        /**
         * Add tangents to the mesh
         * @param tangents The list of tangents to add
         */
        void                    SetTangents(const vector<Vector3>& tangents);

        Vector3*                GetVertices() const { return vertices_; }
        Vector3*                GetNormals() const { return normals_; }
        Vector2*                GetTexCoords() const { return texCoords_; }
        Vector3*                GetTangents() const { return tangents_; }
        size_t                  GetNumVertices() const { return numVertices_; }
        size_t                  GetNumNormals() const { return numNormals_; }
        size_t                  GetNumTexCoords() const { return numTexCoords_; }
        size_t                  GetNumTangents() const { return numTangents_; }

		/**
		 * Get the rendering information about the mesh for rendering
         * @param modelData A list of structures containing the model data
         * @param bufferObjects A pointer to buffer object names
		 */
		void					GetRenderInfo(vector<LoadedModel_t*>*& modelData, unsigned int*& bufferObjects) const;

	protected:
		vector<LoadedModel_t*>*	model_;		/**< The data representing the mesh */

        MeshType_t              meshType_;  /**< The type of the mesh */

        Vector3*                vertices_;  /**< List of vertices */
        Vector3*                normals_;   /**< List of normals */
        Vector2*                texCoords_; /**< List texture coordinates */
        Vector3*                tangents_;  /**< List of tangents */
        size_t                  numVertices_;
        size_t                  numNormals_;
        size_t                  numTexCoords_;
        size_t                  numTangents_;

        // If those variable are set to true, then don't free the memory for the corresponding list
        bool                    referencedVertices_;
        bool                    referencedNormals_;
        bool                    referencedTexCoords_;
        bool                    referencedTangents_;

		// TEMP
		unsigned int*	    vbo_;		/**< Vertex buffer objects */
		unsigned int*	    ibo_;		/**< Index buffer objects */
		unsigned int*   	vao_;		/**< Vertex array objects */
};

}

#endif