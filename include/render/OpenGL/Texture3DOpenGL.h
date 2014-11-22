#ifndef SKETCH_3D_TEXTURE_3D_OPENGL_H
#define SKETCH_3D_TEXTURE_3D_OPENGL_H   

#include "render/Texture3D.h"

#include "render/OpenGL/gl/glew.h"

namespace Sketch3D {

/**
 * @class Texture3DOpenGL
 * 3D texture implementation with OpenGL API
 */
class Texture3DOpenGL : public Texture3D {

    friend class RenderSystemOpenGL;

    public:
		/**
		 * Constructor. Initializes the width, height and depth to 0, the filter mode
		 * to FILTER_MODE_POINT, the wrap mode to WRAP_MODE_CLAMP and the texture
		 * format is set to TEXTURE_FORMAT_RGB24
		 */
								Texture3DOpenGL();

		/**
		 * Constructor
		 * @param width The width of the texture
		 * @param height The height of the texture
         * @param depth The depth of the texture
		 * @param filterMode The filter mode to use
		 * @param wrapMode The wrap mode to use
		 * @param format The format to use
		 */
								Texture3DOpenGL(unsigned int width, unsigned int height, unsigned int depth,
										        FilterMode_t filterMode=FILTER_MODE_LINEAR,
										        WrapMode_t wrapMode=WRAP_MODE_REPEAT,
										        TextureFormat_t format=TEXTURE_FORMAT_RGB24);

        /**
         * Destructor
         */
        virtual                ~Texture3DOpenGL();

        /**
         * Create the actual texture handle
         * @return true if the texture was created correctly
         */
        virtual bool        Create();

    private:
        GLuint              textureName_;   /**< OpenGL texture's name */

        /**
         * Sends the pixel data to the texture object
         */
        virtual void        SetPixelDataBytesImp(unsigned char* data);
        virtual void        SetPixelDataFloatsImp(float* data);

        void                GetOpenglTextureFormat(TextureFormat_t textureFormat, GLuint& internalFormat, GLuint& format, GLuint& type, GLuint& bpp) const;
};

}

#endif
