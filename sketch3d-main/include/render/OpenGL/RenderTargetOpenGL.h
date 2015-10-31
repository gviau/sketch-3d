#ifndef SKETCH_3D_RENDER_TARGET_OPENGL_H
#define SKETCH_3D_RENDER_TARGET_OPENGL_H

#include "render/RenderTarget.h"

#include "render/OpenGL/GL/glew.h"
#include "render/OpenGL/GL/gl.h"

namespace Sketch3D {
class SKETCH_3D_API RenderViewOpenGL {
public:
    RenderViewOpenGL();
    virtual ~RenderViewOpenGL();

    size_t GetTextureName() const;

protected:
    size_t m_TextureName;
};

class SKETCH_3D_API RenderTargetOpenGL : public RenderTarget, public RenderViewOpenGL {
public:
    virtual bool Initialize(size_t width, size_t height, TextureFormat_t textureFormat) override;
};

class SKETCH_3D_API DepthStencilTargetOpenGL : public DepthStencilTarget, public RenderViewOpenGL {
    virtual bool Initialize(size_t width, size_t height, DepthStencilBits_t depthStencilBits) override;
};
}

#endif