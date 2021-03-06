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

    friend class RenderSystemOpenGL;
    friend class RenderTextureOpenGL;
    
    public:
		/**
		 * Constructor. Initializes the width and height to 0, the filter mode
		 * to FILTER_MODE_POINT, the wrap mode to WRAP_MODE_CLAMP and the texture
		 * format is set to TEXTURE_FORMAT_RGB24
         * @param generateMipmaps If set to true, generate mipmaps for this texture
		 */
					        Texture2DOpenGL(bool generateMipmaps=false);

		/**
		 * Constructor
		 * @param width The width of the texture
		 * @param height The height of the texture
         * @param generateMipmaps If set to true, generate mipmaps for this texture
		 * @param filterMode The filter mode to use
		 * @param wrapMode The wrap mode to use
		 * @param format The format to use
		 */
                            Texture2DOpenGL(unsigned int width, unsigned int height, bool generateMipmaps=false,
										    FilterMode_t filterMode=FILTER_MODE_LINEAR,
										    WrapMode_t wrapMode=WRAP_MODE_REPEAT,
										    TextureFormat_t format=TEXTURE_FORMAT_RGB24);

        /**
         * Destructor. Free the OpenGL texture
         */
        virtual            ~Texture2DOpenGL();

        /**
         * Create the actual texture handle
         * @return true if the texture was created correctly
         */
        virtual bool        Create();

        /**
         * Get the data from the texture. If the pointer is null, try to get it from the opengl texture
         */
        virtual const void* GetData() const;

    private:
        GLuint              textureName_;   /**< OpenGL texture's name */

        virtual void        SetFilterModeImpl() const;
        virtual void        SetWrapModeImpl() const;

        /**
         * Sends the pixel data to the texture object
         */
        virtual void        SetPixelDataBytesImp(unsigned char* data);
        virtual void        SetPixelDataFloatsImp(float* data);

        void                GetOpenglTextureFormat(TextureFormat_t textureFormat, GLuint& internalFormat, GLuint& format, GLuint& type, GLuint& bpp) const;
        GLuint              GetOpenglFilterMode() const;
        GLuint              GetOpenglWrapMode() const;
};

}

#endif