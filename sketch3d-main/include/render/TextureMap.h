#ifndef SKETCH_3D_TEXTURE_MAP_H
#define SKETCH_3D_TEXTURE_MAP_H

#include "render/Resource.h"

namespace Sketch3D {
/**
 * @class TextureMap
 * The texture map class holds a texture's data
 */
class SKETCH_3D_API TextureMap : public Resource {
    public:
                        TextureMap(unsigned char* data, size_t width, size_t height, size_t depth=0);
        unsigned char*  GetData() const;
        unsigned char*& GetData();

        size_t          GetWidth() const;
        size_t          GetHeight() const;
        size_t          GetDepth() const;

    private:
        unsigned char*  data_;
        size_t          width_;
        size_t          height_;
        size_t          depth_;
};
}

#endif