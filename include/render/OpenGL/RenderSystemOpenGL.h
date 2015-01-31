#ifndef SKETCH_3D_RENDER_SYSTEM_OPENGL_H
#define SKETCH_3D_RENDER_SYSTEM_OPENGL_H

#include "render/RenderSystem.h"

namespace Sketch3D {

// Forward declaration
class RenderContextOpenGL;

/**
 * @class RenderSystem
 * OpenGL implementation of the render system
 */
class RenderSystemOpenGL : public RenderSystem {
	public:
        RenderSystemOpenGL(Window& window);
		virtual ~RenderSystemOpenGL();
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
        virtual RenderTexture* CreateRenderTexture(unsigned int width, unsigned int height, TextureFormat_t format) const;
        virtual void BindScreenBuffer() const;
        virtual void EnableBlending(bool val) const;
        virtual void SetBlendingEquation(BlendingEquation_t equation) const;
        virtual void SetBlendingFactor(BlendingFactor_t srcFactor, BlendingFactor_t dstFactor) const;
        virtual void BindTexture(const Texture* texture, size_t unit) const;
        virtual void BindShader(const Shader* shader);
        virtual FrustumPlanes_t ExtractViewFrustumPlanes(const Matrix4x4& viewProjection) const;

	private:
		RenderContextOpenGL*	renderContext_;	/**< The render context to create for OpenGL */

		map<const Texture2D*, pair<size_t, size_t>>	textures_;		/**< Texture mapped to the API representation of the texture */

        unsigned int GetBlendingFactor(BlendingFactor_t factor) const;
        virtual void QueryDeviceCapabilities();
};

}

#endif
