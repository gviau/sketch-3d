#ifndef SKETCH_3D_TEXTURE_2D_DIRECT3D9_H
#define SKETCH_3D_TEXTURE_2D_DIRECT3D9_H

#include "render/Texture2D.h"

// Forward class declaration
struct IDirect3DDevice9;
struct IDirect3DTexture9;

namespace Sketch3D {

/**
 * @class Texture2DDirect3D9
 * Direct3D9 implementation of a 2d texture
 */
class Texture2DDirect3D9 : public Texture2D {
    public:
		/**
		 * Constructor. Initializes the width and height to 0, the filter mode
		 * to FILTER_MODE_POINT, the wrap mode to WRAP_MODE_CLAMP and the texture
		 * format is set to TEXTURE_FORMAT_RGB24
         * @param device A pointer to the Direct3D9 device
		 */
					        Texture2DDirect3D9(IDirect3DDevice9* device);

		/**
		 * Constructor
         * @param device A pointer to the Direct3D9 device
		 * @param width The width of the texture
		 * @param height The height of the texture
		 * @param filterMode The filter mode to use
		 * @param wrapMode The wrap mode to use
		 * @param format The format to use
		 */
				            Texture2DDirect3D9(IDirect3DDevice9* device, unsigned int width, unsigned int height,
										       FilterMode_t filterMode=FILTER_MODE_LINEAR,
										       WrapMode_t wrapMode=WRAP_MODE_REPEAT,
										       TextureFormat_t format=TEXTURE_FORMAT_RGB24);

        /**
         * Destructor. Free the Direct3D9 texture
         */
        virtual            ~Texture2DDirect3D9();

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
        IDirect3DDevice9*   device_;
        IDirect3DTexture9*  texture_;   /**< Direct3D9's texture handle */

        /**
         * Sends the pixel data to the texture object
         */
        virtual void        SetPixelDataBytesImp(unsigned char* data);
        virtual void        SetPixelDataFloatsImp(float* data);
};

}

#endif