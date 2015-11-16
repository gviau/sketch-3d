#ifndef SKETCH_3D_RENDER_TARGET_OPENGL_H
#define SKETCH_3D_RENDER_TARGET_OPENGL_H

#include "render/RenderTarget.h"

#include "render/OpenGL/GL/glew.h"
#include "render/OpenGL/GL/gl.h"

namespace Sketch3D {

// Forward class declarations
class Texture2DOpenGL;

class SKETCH_3D_API RenderViewOpenGL {
public:
                        RenderViewOpenGL();
    virtual            ~RenderViewOpenGL();

    Texture2DOpenGL*    GetTexture() const;

protected:
    Texture2DOpenGL*    m_InternalTexture;
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