#ifndef SKETCH_3D_TEXTURE_H
#define SKETCH_3D_TEXTURE_H

namespace Sketch3D {

/**
 * @enum FilterMode_t
 * The filter mode of the texture
 */
enum FilterMode_t {
	FILTER_MODE_POINT,
	FILTER_MODE_BILINEAR,
	FILTER_MODE_TRILINEAR
};

/**
 * @enum WrapMode_t
 * The wrap mode of the texture
 */
enum WrapMode_t {
	WRAP_MODE_CLAMP,
	WRAP_MODE_REPEAT
};

/**
 * @class Texture
 * This class serves as a base class to other classes that act as textures,
 * such as the Texture2D and RenderTexture classes.
 */
class Texture {
	public:
		/**
		 * Constructor. Initializes the width and height to 0, the filter mode
		 * to FILTER_MODE_POINT and the wrap mode to WRAP_MODE_CLAMP
		 */
						Texture();

		/**
		 * Constructor. Initializes the filter mode to FILTER_MODE_POINT and
		 * the wrap mode to WRAP_MODE_CLAMP by default.
		 * @param width The width of the texture
		 * @param height The height of the texture
		 * @param filterMode The filter mode to use
		 * @param wrapMode The wrap mode to use
		 */
						Texture(unsigned int width, unsigned int height,
								FilterMode_t filterMode=FILTER_MODE_BILINEAR,
								WrapMode_t wrapMode=WRAP_MODE_REPEAT);

        /**
         * Destructor
         */
        virtual        ~Texture();

		void			SetWidth(unsigned int width);
		void			SetHeight(unsigned int height);
		void			SetFilterMode(FilterMode_t mode);
		void			SetWrapMode(WrapMode_t mode);

		unsigned int	GetWidth() const;
		unsigned int	GetHeight() const;
		FilterMode_t	GetFilterMode() const;
		WrapMode_t		GetWrapMode() const;

	protected:
		unsigned int	width_;	/**< The width of the texture */
		unsigned int	height_;	/**< The height of the texture */
		FilterMode_t	filterMode_;	/**< The filter mode of the texture */
		WrapMode_t		wrapMode_;	/**< The wrap mode of the texture */
};

}

#endif