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
    typedef map<const Texture2D*, pair<size_t, size_t>> TextureCacheMap_t;

	public:
        RenderSystemOpenGL(Window& window);
		virtual ~RenderSystemOpenGL();
		virtual bool Initialize(const RenderParameters_t& renderParameters);
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
        virtual Shader* CreateShader();
        virtual Texture2D* CreateTexture2D() const;
        virtual Texture3D* CreateTexture3D() const;
        virtual RenderTexture* CreateRenderTexture(unsigned int width, unsigned int height, TextureFormat_t format);
        virtual void BindScreenBuffer() const;
        virtual void EnableBlending(bool val) const;
        virtual void SetBlendingEquation(BlendingEquation_t equation) const;
        virtual void SetBlendingFactor(BlendingFactor_t srcFactor, BlendingFactor_t dstFactor) const;
        virtual size_t BindTexture(const Texture* texture);
        virtual void BindShader(const Shader* shader);
        virtual FrustumPlanes_t ExtractViewFrustumPlanes(const Matrix4x4& viewProjection) const;

	private:
        /**
         * @struct TextureUnitNode_t
         * This texture serves as an element for the texture unit cache. All that it contains is
         * a pointer to the next element and the texture unit it refers to. It is used as a single
         * linked list
         */
        struct TextureUnitNode_t {
            size_t              textureUnit;
            TextureUnitNode_t*  next;
            TextureUnitNode_t*  prev;
        };
        typedef map<size_t, TextureUnitNode_t*> TextureCache_t;

		RenderContextOpenGL*	renderContext_;	/**< The render context to create for OpenGL */

		TextureCacheMap_t	    textures_;		/**< Texture mapped to the API representation of the texture */

        TextureUnitNode_t*      head_;                  /**< Head of the double linked list */
        TextureUnitNode_t*      tail_;                  /**< Tail of the double linked list */
        TextureCache_t          textureCache_;          /**< Texture pointers refer directly to a cache element for faster lookup */

        unsigned int GetBlendingFactor(BlendingFactor_t factor) const;
        virtual void QueryDeviceCapabilities();
        virtual void CreateTextShader();
};

}

#endif
