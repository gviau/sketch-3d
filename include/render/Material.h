#ifndef SKETCH_3D_MATERIAL_H
#define SKETCH_3D_MATERIAL_H

namespace Sketch3D {

// Forward declarations
class Mesh;
class Shader;

/**
 * @class Material
 * This class defines a material to use on a mesh. It gives has a shader that
 * will be run to render the mesh as well as a set of general material
 * properties that will be pass to the shader to control how the mesh will be
 * rendered.
 */
class Material {
	public:
		/**
		 * Constructor. Sets the shader for the material
		 */
					Material(Shader* shader);

		/**
		 * Apply the material for the next render on a mesh
		 * @param mesh The mesh on which to apply the material
		 */
		void		Apply(Mesh& mesh) const;

		void		SetShader(Shader* shader);
		Shader*		GetShader() const;

	private:
		Shader*		shader_;	/**< Shader used by the material */
};

}

#endif