#ifndef SKETCH_3D_RENDER_SYSTEM_DIRECT3D9_H
#define SKETCH_3D_RENDER_SYSTEM_DIRECT3D9_H

#include "render/RenderSystem.h"

#include "render/Direct3D9/RenderContextDirect3D9.h"

#include <d3d9.h>

namespace Sketch3D {

/**
 * @class RenderSystemDirect3D9
 * Direct3D9 implementation of the renderer
 */
class RenderSystemDirect3D9 : public RenderSystem {
    public:
        RenderSystemDirect3D9(Window& window);
        ~RenderSystemDirect3D9();
        virtual bool Initialize();
        virtual void SetClearColor(float red, float green, float blue, float alpha=1.0f);
        virtual void Clear(int buffer) const;
        virtual void StartRender();
        virtual void EndRender();
        virtual void PresentFrame();
        virtual Matrix4x4 OrthoProjection(float left, float right, float top, float bottom, float nearPlane, float farPlane) const;
        virtual Matrix4x4 PerspectiveProjection(float left, float right, float top, float bottom, float nearPlane, float farPlane) const;
        virtual void SetRenderFillMode(RenderMode_t mode) const;
        virtual void SetViewport(size_t x, size_t y, size_t width, size_t height);
        virtual void EnableDepthTest(bool val) const;
        virtual void EnableDepthWrite(bool val) const;
        virtual void EnableColorWrite(bool val) const;
        virtual void SetDepthComparisonFunc(DepthFunc_t comparison) const;
        virtual void SetCullingMethod(CullingMethod_t cullingMethod) const;
        virtual Shader* CreateShader(const string& vertexFilename, const string& fragmentFilename);
        virtual Texture2D* CreateTexture2D() const;
        virtual Texture3D* CreateTexture3D() const;
        virtual RenderTexture* CreateRenderTexture(unsigned int width, unsigned int height, TextureFormat_t format);
        virtual void BindScreenBuffer() const;
        virtual void EnableBlending(bool val) const;
        virtual void SetBlendingEquation(BlendingEquation_t equation) const;
        virtual void SetBlendingFactor(BlendingFactor_t srcFactor, BlendingFactor_t destFactor) const;
        virtual void BindTexture(const Texture* texture, size_t unit) const;
        virtual void BindShader(const Shader* shader);
        virtual FrustumPlanes_t ExtractViewFrustumPlanes(const Matrix4x4& viewProjection) const;

    private:
        RenderContextDirect3D9* renderContext_; /**< The render context to create for Direct3D9 */
        IDirect3DDevice9*       device_;    /**< Rendering device */
        IDirect3DSurface9*      depthBuffer_;   /**< The normal depth buffer used for rendering */
        IDirect3DSurface9*      renderTarget_;  /**< The normal render target used for rendering */

        D3DCOLORVALUE           clearColor_;    /**< Clear color of the buffer */

        unsigned int GetBlendingFactor(BlendingFactor_t factor) const;
        virtual void QueryDeviceCapabilities();
};

}

#endif