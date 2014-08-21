#ifndef SKETCH_3D_TEXTURE_2D_OPENGL_H
#define SKETCH_3D_TEXTURE_2D_OPENGL_H

#include "render/Texture2D.h"

#include "gl/glew.h"

namespace Sketch3D {

/**
 * @class Texture2DOpenGL
 * 2D texture implementation with OpenGL API
 */
class Texture2DOpenGL : public Texture2D {

    friend class RenderTextureOpenGL;

    public:
		/**
		 * Constructor. Initializes the width and height to 0, the filter mode
		 * to FILTER_MODE_POINT, the wrap mode to WRAP_MODE_CLAMP and the texture
		 * format is set to TEXTURE_FORMAT_RGB24
		 */
					    Texture2DOpenGL();

		/**
		 * Constructor
		 * @param width The width of the texture
		 * @param height The height of the texture
		 * @param filterMode The filter mode to use
		 * @param wrapMode The wrap mode to use
		 * @param format The format to use
		 */
				        Texture2DOpenGL(unsigned int width, unsigned int height,
										  FilterMode_t filterMode=FILTER_MODE_BILINEAR,
										  WrapMode_t wrapMode=WRAP_MODE_REPEAT,
										  TextureFormat_t format=TEXTURE_FORMAT_RGB24);

        /**
         * Destructor. Free the OpenGL texture
         */
        virtual        ~Texture2DOpenGL();

        /**
         * Create the actual texture handle
         * @return true if the texture was created correctly
         */
        virtual bool    Create();

        /**
         * Bind the texture to a texture unit
         * @param unit On which texture unit should we bind the texture
         */
        virtual void    Bind(unsigned int unit);

    private:
        GLuint          textureName_;   /**< OpenGL texture's name */

        /**
         * Sends the pixel data to the texture object
         */
        virtual void    SetPixelDataImp(unsigned char* data);
};

}

#endif