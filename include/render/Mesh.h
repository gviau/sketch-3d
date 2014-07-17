#ifndef SKETCH_3D_MESH_H
#define SKETCH_3D_MESH_H

#include "render/ResourceManager.h"

#include <string>
#include <vector>
using namespace std;

namespace Sketch3D {

/**
 * @class Mesh
 * This class holds the representation of a 3D model. It is reponsible to send
 * the needed data to the underlying rendering system.
 */
class Mesh {
	public:
		/**
		 * Constructor. Read a mesh from a file and load the data in memory
		 * @param filename The name of the file containing the model data
		 */
								Mesh(const string& filename);

        /**
         * Destructor - release the buffer objects
         */
                               ~Mesh();

		/**
		 * Render the model
		 */
		void					Render() const;

	protected:
		vector<LoadedModel_t*>*	model_;		/**< The data representing the mesh */

		// TEMP
		unsigned int*	    vbo_;		/**< Vertex buffer objects */
		unsigned int*	    ibo_;		/**< Index buffer objects */
		unsigned int*   	vao_;		/**< Vertex array objects */

		/**
		 * Private constructor. Used to create prefabs
		 */
								Mesh();
};

}

#endif