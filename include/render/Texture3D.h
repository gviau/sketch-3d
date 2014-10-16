#ifndef SKETCH_3D_TEXTURE_3D_H
#define SKETCH_3D_TEXTURE_3D_H

#include "render/Texture.h"

namespace Sketch3D {

/**
 * @class Texture3D
 * This class implements the functionalities that are specific to a 3d texture
 */
class Texture3D : public Texture {
    public:
		/**
		 * Constructor. Initializes the width, height and depth to 0, the filter mode
		 * to FILTER_MODE_POINT, the wrap mode to WRAP_MODE_CLAMP and the texture
		 * format is set to TEXTURE_FORMAT_RGB24
		 */
								Texture3D();

		/**
		 * Constructor
		 * @param width The width of the texture
		 * @param height The height of the texture
         * @param depth The depth of the texture
		 * @param filterMode The filter mode to use
		 * @param wrapMode The wrap mode to use
		 * @param format The format to use
		 */
								Texture3D(unsigned int width, unsigned int height, unsigned int depth,
										  FilterMode_t filterMode=FILTER_MODE_NEAREST,
										  WrapMode_t wrapMode=WRAP_MODE_REPEAT,
										  TextureFormat_t format=TEXTURE_FORMAT_RGB24);

        /**
         * Destructor
         */
        virtual                ~Texture3D();

        /**
         * Create the actual texture handle
         * @return true if the texture was created correctly
         */
        virtual bool            Create() = 0;

        /**
         * Activate the texture
         * @param unit On which texture unit should we bind the texture
         */
        virtual void            Bind(unsigned int unit) = 0;

        /**
         * Set the data as an array of bytes. Will only work with texture formats that doesn't require floats
         * @param data The new data array
         * @param width The width of the new data. Must be of the same width than this texture
         * @param height The height of the new data. Must be of the same height than this texture
         * @param depth The depth of the new data. Must be of the same depth than this texture
         * @return true if the operation was succesful, false otherwise
         */
        bool                    SetPixelDataBytes(unsigned char* data, size_t width, size_t height, size_t depth);

        /**
         * Set the data as an array of floats. Will only work with texture formats that doesn't require bytes
         * @param data The new data array
         * @param width The width of the new data. Must be of the same width than this texture
         * @param height The height of the new data. Must be of the same height than this texture
         * @param depth The depth of the new data. Must be of the same depth than this texture
         * @return true if the operation was succesful, false otherwise
         */
        bool                    SetPixelDataFloats(float* data, size_t width, size_t height, size_t depth);

        void                    SetDepth(size_t depth) { depth_ = depth; }

        size_t                  GetDepth() const { return depth_; }
        const void*             GetData() const { return data_; }

    protected:
        size_t                  depth_; /**< The depth of the texture */
        void*                   data_;  /**< The actual texture data */

        /**
         * Sends the data to the texture object
         */
        virtual void            SetPixelDataBytesImp(unsigned char* data) = 0;
        virtual void            SetPixelDataFloatsImp(float* data) = 0;
};

}

#endif