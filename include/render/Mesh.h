#ifndef SKETCH_3D_MESH_H
#define SKETCH_3D_MESH_H

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
		 * Render the model
		 */
		void					Render() const;

		//  USEFUL STATIC FUNCTIONS
		/**
		 * Create a mesh representing an unit cube
		 * @param width The width of the cube
		 * @param height The height of the cube
		 * @param depth The depth of the cube
		 * @return a newly created mesh
		 */
		static Mesh				CreateCube(float width, float height, float depth);

	protected:
		vector<float>			vertices_;	/**< The vertices that compose the model */
		vector<unsigned int>	indices_;	/**< The indices of the model */

		// TEMP
		unsigned int			vbo_;		/**< Vertex buffer object */
		unsigned int			ibo_;		/**< Index buffer object */
		unsigned int			vao_;		/**< Vertex array object */

		/**
		 * Private constructor. Used to create prefabs
		 */
								Mesh();
};

}

#endif