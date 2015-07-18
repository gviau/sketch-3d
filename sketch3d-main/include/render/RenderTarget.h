#ifndef SKETCH_3D_RENDER_TARGET_H
#define SKETCH_3D_RENDER_TARGET_H

#include "render/Texture.h"

#include "render/Renderer_Common.h"

namespace Sketch3D {
class SKETCH_3D_API RenderTarget : public HardwareResource {
    public:
        virtual        ~RenderTarget() {}

        virtual bool    Initialize(size_t width, size_t height, TextureFormat_t textureFormat) = 0;

        size_t          GetWidth() const;
        size_t          GetHeight() const;
        TextureFormat_t GetFormat() const;

    protected:
        size_t          width_;
        size_t          height_;
        TextureFormat_t format_;
};

class SKETCH_3D_API DepthStencilTarget : public HardwareResource {
    public:
        virtual        ~DepthStencilTarget() {}

        virtual bool    Initialize(size_t width, size_t height, DepthStencilBits_t depthStencilBits) = 0;

        size_t          GetWidth() const;
        size_t          GetHeight() const;

    protected:
        size_t          width_;
        size_t          height_;
};
}

#endif