#ifndef SKETCH_3D_TEXTURE_2D_H
#define SKETCH_3D_TEXTURE_2D_H

#include "render/Texture.h"

#include <string>
#include <vector>
using namespace std;

namespace Sketch3D {

// Forward declaration
class RenderSystemOpenGL;
class ResourceManager;
	
/**
 * @enum TextureFormat_t
 * The format of the texture, describing how to interpret the bytes
 */
enum TextureFormat_t {
	TEXTURE_FORMAT_RGB24,
	TEXTURE_FORMAT_RGBA32
};

/**
 * @class Texture2D
 * This class implements functionnalities that are specific to a texture that
 * is to be applied on a polygon
 */
class Texture2D : public Texture {

	friend class RenderSystemOpenGL;
    friend class ResourceManager;

	public:
		/**
		 * Constructor. Initializes the width and height to 0, the filter mode
		 * to FILTER_MODE_POINT, the wrap mode to WRAP_MODE_CLAMP and the texture
		 * format is set to TEXTURE_FORMAT_RGB24
		 */
								Texture2D();

		/**
		 * Constructor
		 * @param width The width of the texture
		 * @param height The height of the texture
		 * @param filterMode The filter mode to use
		 * @param wrapMode The wrap mode to use
		 * @param format The format to use
		 */
								Texture2D(unsigned int width, unsigned int height,
										  FilterMode_t filterMode=FILTER_MODE_BILINEAR,
										  WrapMode_t wrapMode=WRAP_MODE_REPEAT,
										  TextureFormat_t format=TEXTURE_FORMAT_RGB24);

		void					SetTextureFormat(TextureFormat_t format);
		TextureFormat_t			GetTextureFormat() const;

        const unsigned char* GetData() const { return data_; }

	private:
		TextureFormat_t			format_;	/**< The format of the texture */
		unsigned char*	        data_;		/**< The actual texture data */
};

}

#endif