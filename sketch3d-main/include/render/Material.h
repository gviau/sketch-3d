#ifndef SKETCH_3D_MATERIAL_H
#define SKETCH_3D_MATERIAL_H

#include "system/Platform.h"

#include <map>
#include <string>
using namespace std;

namespace Sketch3D {

// Forward declarations
class Mesh;
class Shader;
class Texture;

enum TransluencyType_t {
    TRANSLUENCY_TYPE_OPAQUE,
    TRANSLUENCY_TYPE_ADDITIVE
};

/**
 * @class Material
 * This class defines a material to use on a mesh. It gives has a shader that
 * will be run to render the mesh as well as a set of general material
 * properties that will be pass to the shader to control how the mesh will be
 * rendered.
 */
class SKETCH_3D_API Material {
	public:
		/**
		 * Constructor. Sets the shader for the material
		 */
					                        Material(Shader* shader);

        /**
         * Add a texture mapped with a name for the shader. Those textures will be used on every surface.
         * This allows a material to specify special textures that may not be part of the original mesh.
         * @param textureName The name to give to this texture to reference it in the shader
         * @param texture The texture to use
         * @return false if the texture that the user added overwritted a previous texture with the same name, true otherwise
         */
        bool                                AddTexture(const string& textureName, Texture* texture);

		void		                        SetShader(Shader* shader);
        void                                SetTransluencyType(TransluencyType_t type);

        const map<string, Texture*>&        GetTextures() const;
		Shader*		                        GetShader() const;
        TransluencyType_t                   GetTransluencyType() const;

	private:
        map<string, Texture*>               textures_;  /**< Special texture to set for this material */
		Shader*		                        shader_;	/**< Shader used by the material */
        TransluencyType_t                   transluencyType_;   /**< The transluency type for this material */
};

}

#endif