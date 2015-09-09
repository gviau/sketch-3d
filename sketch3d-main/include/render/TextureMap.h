#ifndef SKETCH_3D_TEXTURE_MAP_H
#define SKETCH_3D_TEXTURE_MAP_H

#include "render/Resource.h"

#include "render/Texture.h"

#include <memory>
#include <string>
using namespace std;

namespace Sketch3D
{

/**
 * @class TextureMap
 * The texture map class holds a texture's data
 */
class SKETCH_3D_API TextureMap : public Resource
{
    public:
                        TextureMap(unsigned char* data, size_t width, size_t height, size_t depth=0);
                       ~TextureMap();
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

bool LoadTextureMapFromFile(const string& filename, shared_ptr<TextureMap>& textureMap, TextureFormat_t& textureFormat);

}

#endif