#ifndef SKETCH_3D_MATERIAL_H
#define SKETCH_3D_MATERIAL_H

#include <vector>
using namespace std;

namespace Sketch3D {

// Forward declarations
class Mesh;
class Shader;
class Texture2D;

enum TransluencyType_t {
    TRANSLUENCY_TYPE_OPAQUE,
    TRANSLUENCY_TYPE_ADDITIVE,
    TRANSLUENCY_TYPE_SUBTRACTIVE
};

// TODO
// support more than one textures per mesh

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

		void		                        SetShader(Shader* shader);
	    void                                SetTextures(vector<vector<Texture2D*>>*& textures);
        void                                SetTransluencyType(TransluencyType_t type);

		Shader*		                        GetShader() const;
        const vector<vector<Texture2D*>>*   GetTextures() const;
        TransluencyType_t                   GetTransluencyType() const;

	private:
		Shader*		                        shader_;	/**< Shader used by the material */
        vector<vector<Texture2D*>>*         textures_;  /**< Textures used to render a mesh. Those are ordered to go with a specific mesh */
        TransluencyType_t                   transluencyType_;   /**< The transluency type for this material */
};

}

#endif