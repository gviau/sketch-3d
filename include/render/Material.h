#ifndef SKETCH_3D_MATERIAL_H
#define SKETCH_3D_MATERIAL_H

namespace Sketch3D {

// Forward declarations
class Mesh;
class Shader;
class Texture2D;

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
		
		/**
		 * Set one of the four textures of the material.
		 * @param num The texture number to set. This parameter has to be in
		 * [1, 4], otherwise the set will silently fail.
		 * @param texture The texture to set
		 */
		void		SetTexture(int num, Texture2D* texture);

		Shader*		GetShader() const;
		Texture2D*	GetTexture(int num) const;

	private:
		Shader*		shader_;	/**< Shader used by the material */
		Texture2D*	texture1_;	/**< First texture of the material */
		Texture2D*	texture2_;	/**< Second texture of the material */
		Texture2D*	texture3_;	/**< Third texture of the material */
		Texture2D*	texture4_;	/**< Fourth texture of the material */
};

}

#endif