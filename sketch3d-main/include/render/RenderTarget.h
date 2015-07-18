#ifndef SKETCH_3D_RENDER_TARGET_H
#define SKETCH_3D_RENDER_TARGET_H

#include "render/Texture.h"

namespace Sketch3D {
class SKETCH_3D_API RenderTarget : public HardwareResource {
    public:
                        RenderTarget(size_t width, size_t height, TextureFormat_t textureFormat);
        virtual        ~RenderTarget() {}

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
                        DepthStencilTarget(size_t width, size_t height);
        virtual        ~DepthStencilTarget() {}

        size_t          GetWidth() const;
        size_t          GetHeight() const;

    protected:
        size_t          width_;
        size_t          height_;
};
}

#endif