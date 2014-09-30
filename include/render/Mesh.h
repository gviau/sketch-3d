#ifndef SKETCH_3D_MESH_H
#define SKETCH_3D_MESH_H

#include "math/Vector2.h"
#include "math/Vector3.h"

#include "render/ResourceManager.h"

#include <string>
#include <vector>
using namespace std;

// Forward declaration
namespace Assimp {
	class Importer;
}

namespace Sketch3D {

// Forward declaration
class Material;

/**
 * @struct SurfaceTriangles_t
 * Structure containing the information about a surface to draw regarding the vertices
 */
struct SurfaceTriangles_t {
                    SurfaceTriangles_t() : vertices(nullptr), normals(nullptr), texCoords(nullptr), tangents(nullptr),
                                           indices(nullptr), numVertices(0), numNormals(0), numTexCoords(0), numTangents(0),
                                           numIndices(0) {}

    Vector3*        vertices;   /**< List of vertices */
    Vector3*        normals;    /**< List of normals */
    Vector2*        texCoords;  /**< List texture coordinates */
    Vector3*        tangents;   /**< List of tangents */
    unsigned short* indices;    /**< List of indices */
    size_t          numVertices;
    size_t          numNormals;
    size_t          numTexCoords;
    size_t          numTangents;
    size_t          numIndices;
};

/**
 * @struct ModelSurface_t
 * Struct representing a surface geometry data paired with a material
 */
struct ModelSurface_t {
    int                 id;
    const Material*     material;
    SurfaceTriangles_t* geometry;
};

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
        void                    Load(const string& filename, MeshType_t meshType=MESH_TYPE_STATIC);

        /**
         * Add a model surface to the mesh
         * @param modelSurface The model surface to add
         */
        void                    AddSurface(const ModelSurface_t& surface);

        /**
         * Initialize the mesh with geometry data
         * @param meshType The type of mesh that will be rendered, static or dynamic
         */
        void                    Initialize(MeshType_t meshType=MESH_TYPE_STATIC);

        /**
         * If the mesh is a dynamic mesh, re-uploads the mesh data
         */
        void                    UpdateMeshData() const;

		/**
		 * Get the rendering information about the mesh for rendering
         * @param bufferObjects A pointer to buffer object names
         * @param surfaces The list of surfaces
		 */
        void					GetRenderInfo(unsigned int*& bufferObjects, vector<ModelSurface_t>& surfaces) const;

	protected:
        MeshType_t              meshType_;  /**< The type of the mesh */
        vector<ModelSurface_t>  surfaces_;  /**< List of surfaces for the model */
        string                  filename_;  /**< The name of the file loaded, if we loaded it from a file */
        bool                    fromCache_; /**< Set to true if the model is cached, false otherwise */
        Assimp::Importer*       importer_;  /**< Importer used to load a model from a file */

		// TEMP
		unsigned int*	        vbo_;		/**< Vertex buffer objects */
		unsigned int*	        ibo_;		/**< Index buffer objects */
		unsigned int*   	    vao_;		/**< Vertex array objects */
};

}

#endif