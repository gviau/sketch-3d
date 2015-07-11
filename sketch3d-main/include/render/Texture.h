#ifndef SKETCH_3D_TEXTURE_H
#define SKETCH_3D_TEXTURE_H

#include "render/HardwareResource.h"

namespace Sketch3D {

// Forward class declarations
class TextureMap;

/**
 * @enum TextureFormat_t
 * The format of the texture, describing how to interpret the bytes
 */
enum TextureFormat_t {
    GRAYSCALE,

	RGB24,
	RGBA32,

    // There should be a separation between byte texture format and floating point texture format
    R32F,
    RG32F,
    RGBA32F,
    R16F,
    RG16F,
    RGBA16F,

    // Special texture formats
    DEPTH
};

enum TextureType_t {
    TYPE_1D,
    TYPE_2D,
    TYPE_3D
};

/**
 * @class Texture
 * This class serves as a base class to other classes that act as textures
 */
class SKETCH_3D_API Texture : public HardwareResource {
    public:
        /**
         * Constructor
         * @param textureMap The texture map to use to construct the texture
         * @param textureFormat The format to use for this texture i.e. how to interpret the data in the texture map
         * @param dynamic Set to true if the texture can be modified on the CPU
         * @param immutable Set to true if the texture cannot be modified on the CPU. Will reside on the GPU
         */
                        Texture(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable=false);

        TextureMap*     GetTextureMap() const;
        TextureFormat_t GetTextureFormat() const;

    protected:
        TextureMap*     textureMap_;
        TextureFormat_t textureFormat_;
};

class SKETCH_3D_API Texture1D : public Texture {
    public:
        Texture1D(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable=false);
};

class SKETCH_3D_API Texture2D : public Texture {
    public:
        Texture2D(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable=false);
};

class SKETCH_3D_API Texture3D : public Texture {
    public:
        Texture3D(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable=false);
};

size_t GetRowPitch(size_t width, TextureFormat_t textureFormat);
size_t Get2DSlicePitch(size_t width, size_t height, TextureFormat_t textureFormat);

}

#endif
