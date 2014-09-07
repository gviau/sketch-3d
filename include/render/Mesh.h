#ifndef SKETCH_3D_MESH_H
#define SKETCH_3D_MESH_H

#include "render/ResourceManager.h"

#include <string>
#include <vector>
using namespace std;

namespace Sketch3D {

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
         * Initialize the mesh with geometry data
         * @param modelData A pointer to a vector of pointers to a structure containing the model data
         * @param meshType The type of mesh that will be rendered, static or dynamic
         */
        void                    Initialize(vector<LoadedModel_t*>*& modelData, MeshType_t meshType=MESH_TYPE_STATIC);

        /**
         * Updates the mesh with new model data. This only works for dynamic mesh
         * @param modelData The new model data to use
         * @return false if the mesh is static, true otherwise
         */
        bool                    UpdateMeshData(vector<LoadedModel_t*>*& modelData);

		/**
		 * Get the rendering information about the mesh for rendering
         * @param modelData A list of structures containing the model data
         * @param bufferObjects A pointer to buffer object names
		 */
		void					GetRenderInfo(vector<LoadedModel_t*>*& modelData, unsigned int*& bufferObjects) const;

	protected:
		vector<LoadedModel_t*>*	model_;		/**< The data representing the mesh */

        MeshType_t              meshType_;  /**< The type of the mesh */

		// TEMP
		unsigned int*	    vbo_;		/**< Vertex buffer objects */
		unsigned int*	    ibo_;		/**< Index buffer objects */
		unsigned int*   	vao_;		/**< Vertex array objects */
};

}

#endif