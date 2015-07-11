#ifndef SKETCH_3D_TEXTURE_DIRECT3D11
#define SKETCH_3D_TEXTURE_DIRECT3D11

#include "render/Texture.h"

#pragma warning( disable : 4005 )

#include <d3d11.h>

#pragma warning( default : 4005 )

namespace Sketch3D {
class SKETCH_3D_API TextureDirect3D11 {
    public:
        TextureDirect3D11(ID3D11Device* device);
};

class SKETCH_3D_API Texture1DDirect3D11 : public Texture1D, public TextureDirect3D11 {
    public:
                            Texture1DDirect3D11(ID3D11Device* device, TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable=false);
                           ~Texture1DDirect3D11();

        ID3D11Texture1D*    GetTexture() const;

    private:
        ID3D11Texture1D*    texture_;
};

class SKETCH_3D_API Texture2DDirect3D11 : public Texture2D, public TextureDirect3D11 {
    public:
                            Texture2DDirect3D11(ID3D11Device* device, TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable=false);
                           ~Texture2DDirect3D11();

        ID3D11Texture2D*    GetTexture() const;

    private:
        ID3D11Texture2D*    texture_;
};

class SKETCH_3D_API Texture3DDirect3D11 : public Texture3D, public TextureDirect3D11 {
    public:
                            Texture3DDirect3D11(ID3D11Device* device, TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable=false);
                           ~Texture3DDirect3D11();

        ID3D11Texture3D*    GetTexture() const;

    private:
        ID3D11Texture3D*    texture_;
};

DXGI_FORMAT GetD3DTextureFormat(TextureFormat_t textureFormat);
}

#endif