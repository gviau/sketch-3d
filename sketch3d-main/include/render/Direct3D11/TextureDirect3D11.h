#ifndef SKETCH_3D_TEXTURE_DIRECT3D11_H
#define SKETCH_3D_TEXTURE_DIRECT3D11_H

#include "render/Texture.h"

#pragma warning( disable : 4005 )

#include <d3d11.h>

#pragma warning( default : 4005 )

namespace Sketch3D {
class SKETCH_3D_API TextureDirect3D11 {
    public:
                                    TextureDirect3D11(ID3D11Device* device);
        virtual                    ~TextureDirect3D11();

        ID3D11ShaderResourceView*   GetShaderResourceView() const;

    protected:
        ID3D11Device*               device_;
        ID3D11ShaderResourceView*   shaderResourceView_;

        bool                        CreateShaderResourceView(ID3D11Resource* resource, TextureFormat_t format, TextureType_t textureType);
};

class SKETCH_3D_API Texture1DDirect3D11 : public Texture1D, public TextureDirect3D11 {
    public:
                            Texture1DDirect3D11(ID3D11Device* device);
        virtual            ~Texture1DDirect3D11();

        virtual bool        Initialize(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable=false) override;
        ID3D11Texture1D*    GetTexture() const;

    private:
        ID3D11Texture1D*    texture_;
};

class SKETCH_3D_API Texture2DDirect3D11 : public Texture2D, public TextureDirect3D11 {
    public:
                            Texture2DDirect3D11(ID3D11Device* device);
        virtual            ~Texture2DDirect3D11();

        virtual bool        Initialize(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable=false) override;
        ID3D11Texture2D*    GetTexture() const;

    private:
        ID3D11Texture2D*    texture_;
};

class SKETCH_3D_API Texture3DDirect3D11 : public Texture3D, public TextureDirect3D11 {
    public:
                            Texture3DDirect3D11(ID3D11Device* device);
        virtual            ~Texture3DDirect3D11();

        virtual bool        Initialize(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable=false) override;
        ID3D11Texture3D*    GetTexture() const;

    private:
        ID3D11Texture3D*    texture_;
};

DXGI_FORMAT GetD3DTextureFormat(TextureFormat_t textureFormat);
}

#endif