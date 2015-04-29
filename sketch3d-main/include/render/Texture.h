#ifndef SKETCH_3D_TEXTURE_H
#define SKETCH_3D_TEXTURE_H

#include "system/Platform.h"

namespace Sketch3D {

/**
 * @enum FilterMode_t
 * The filter mode of the texture
 */
enum FilterMode_t {
	FILTER_MODE_NEAREST,
    FILTER_MODE_LINEAR
};

/**
 * @enum WrapMode_t
 * The wrap mode of the texture
 */
enum WrapMode_t {
	WRAP_MODE_CLAMP,
	WRAP_MODE_REPEAT,
    WRAP_MODE_CLAMP_TO_BORDER
};

/**
 * @enum TextureFormat_t
 * The format of the texture, describing how to interpret the bytes
 */
enum TextureFormat_t {
    TEXTURE_FORMAT_GRAYSCALE,

	TEXTURE_FORMAT_RGB24,
	TEXTURE_FORMAT_RGBA32,
    TEXTURE_FORMAT_BGR24,
    TEXTURE_FORMAT_BGRA32,

    // There should be a separation between byte texture format and floating point texture format
    TEXTURE_FORMAT_R32F,
    TEXTURE_FORMAT_RG32F,
    TEXTURE_FORMAT_RGBA32F,
    TEXTURE_FORMAT_R16F,
    TEXTURE_FORMAT_RG16F,
    TEXTURE_FORMAT_RGBA16F,

    // Special texture formats
    TEXTURE_FORMAT_DEPTH
};

/**
 * @enum TextureType_t
 * Type of the texture (2D or 3D)
 */
enum TextureType_t {
    TEXTURE_TYPE_2D,
    TEXTURE_TYPE_3D
};

/**
 * @class Texture
 * This class serves as a base class to other classes that act as textures,
 * such as the Texture2D and RenderTexture classes.
 */
class SKETCH_3D_API Texture {
	public:
		/**
		 * Constructor. Initializes the width and height to 0, the filter mode
		 * to FILTER_MODE_POINT and the wrap mode to WRAP_MODE_CLAMP and the texture
		 * format is set to TEXTURE_FORMAT_RGB24
         * @param generateMipmaps If set to true, generate mipmaps for this texture
		 */
                                Texture(bool generateMipmaps=false);

		/**
		 * Constructor. Initializes the filter mode to FILTER_MODE_POINT and
		 * the wrap mode to WRAP_MODE_CLAMP by default.
		 * @param width The width of the texture
		 * @param height The height of the texture
         * @param generateMipmaps If set to true, generate mipmaps for this texture
		 * @param filterMode The filter mode to use
		 * @param wrapMode The wrap mode to use
         * @param format The format to use
		 */
						        Texture(unsigned int width, unsigned int height, bool generateMipmaps=false,
								        FilterMode_t filterMode=FILTER_MODE_LINEAR,
								        WrapMode_t wrapMode=WRAP_MODE_REPEAT,
								        TextureFormat_t format=TEXTURE_FORMAT_RGB24);

        /**
         * Destructor
         */
        virtual                ~Texture();

        /**
         * Create the actual texture handle
         * @return true if the texture was created correctly
         */
        virtual bool            Create() = 0;

        /**
         * Activate the texture
         * @return The texture unit on which the texture was bound
         */
        unsigned int            Bind() const;

		void			        SetWidth(unsigned int width);
		void			        SetHeight(unsigned int height);
        void                    SetGenerateMipmaps(bool generateMipmaps);
		void			        SetFilterMode(FilterMode_t mode);
		void			        SetWrapMode(WrapMode_t mode);
        void		            SetTextureFormat(TextureFormat_t format);

		unsigned int	        GetWidth() const;
		unsigned int	        GetHeight() const;
        bool                    GetGenerateMipmaps() const;
		FilterMode_t	        GetFilterMode() const;
		WrapMode_t		        GetWrapMode() const;
        TextureFormat_t	        GetTextureFormat() const;

        virtual TextureType_t   GetType() const = 0;

	protected:
		unsigned int	        width_;	/**< The width of the texture */
		unsigned int	        height_;	/**< The height of the texture */
        bool                    generateMipmaps_;   /**< Should we generate mipmaps for this texture */
		FilterMode_t	        filterMode_;	/**< The filter mode of the texture */
		WrapMode_t		        wrapMode_;	/**< The wrap mode of the texture */
        TextureFormat_t         format_;    /**< The texture format */

        virtual void            SetFilterModeImpl() const = 0;
        virtual void            SetWrapModeImpl() const = 0;
};

}

#endif
