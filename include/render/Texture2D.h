#ifndef SKETCH_3D_TEXTURE_2D_H
#define SKETCH_3D_TEXTURE_2D_H

#include "render/Texture.h"

#include <stdint.h>
#include <string>
#include <vector>
using namespace std;

namespace Sketch3D {

#define MAX_TEXTURE_ID 1048576  // 20 bits max

// Forward declaration
class RenderSystemOpenGL;

/**
 * @class Texture2D
 * This class implements functionnalities that are specific to a texture that
 * is to be applied on a polygon
 */
class Texture2D : public Texture {

	public:
		/**
		 * Constructor. Initializes the width and height to 0, the filter mode
		 * to FILTER_MODE_POINT, the wrap mode to WRAP_MODE_CLAMP and the texture
		 * format is set to TEXTURE_FORMAT_RGB24
         * @param generateMipmaps If set to true, generate mipmaps for this texture
		 */
								Texture2D(bool generateMipmaps=false);

		/**
		 * Constructor
		 * @param width The width of the texture
		 * @param height The height of the texture
         * @param generateMipmaps If set to true, generate mipmaps for this texture
		 * @param filterMode The filter mode to use
		 * @param wrapMode The wrap mode to use
		 * @param format The format to use
		 */
                                Texture2D(unsigned int width, unsigned int height, bool generateMipmaps=false,
										  FilterMode_t filterMode=FILTER_MODE_LINEAR,
										  WrapMode_t wrapMode=WRAP_MODE_REPEAT,
										  TextureFormat_t format=TEXTURE_FORMAT_RGB24);

        /**
         * Destructor
         */
        virtual                ~Texture2D();

        /**
         * Load the texture from a file
         * @param filename The name of file from which the texture will be loaded
         * @return true if the texture was loaded, false otherwise
         */
        bool                    Load(const string& filename);

        /**
         * Create the actual texture handle
         * @return true if the texture was created correctly
         */
        virtual bool            Create() = 0;

        /**
         * Set the data as an array of bytes. Will only work with texture formats that doesn't require floats
         * @param data The new data array
         * @param width The width of the new data. Must be of the same width than this texture
         * @param height The height of the new data. Must be of the same height than this texture
         * @return true if the operation was succesful, false otherwise
         */
        bool                    SetPixelDataBytes(unsigned char* data, size_t width, size_t height);

        /**
         * Set the data as an array of floats. Will only work with texture formats that doesn't require bytes
         * @param data The new data array
         * @param width The width of the new data. Must be of the same width than this texture
         * @param height The height of the new data. Must be of the same height than this texture
         * @return true if the operation was succesful, false otherwise
         */
        bool                    SetPixelDataFloats(float* data, size_t width, size_t height);

        string                  GetFilename() const { return filename_; }

        virtual const void*     GetData() const = 0;
        uint32_t                GetId() const { return id_; }

        virtual TextureType_t   GetType() const { return TEXTURE_TYPE_2D; }

	protected:
        string                  filename_;  /**< The name of the loaded file */
		void*	                data_;		/**< The actual texture data */
        uint32_t                id_;        /**< Id of the texture */
        bool                    fromCache_; /**< Set to true if the texture is cached, false otherwise */

        static uint32_t         nextAvailableId_;

        virtual void            SetFilterModeImpl() const = 0;
        virtual void            SetWrapModeImpl() const = 0;

        /**
         * Sends the data to the texture object
         */
        virtual void            SetPixelDataBytesImp(unsigned char* data) = 0;
        virtual void            SetPixelDataFloatsImp(float* data) = 0;
};

}

#endif