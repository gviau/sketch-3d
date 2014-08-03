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
		~RenderSystemOpenGL();
		virtual bool Initialize();
		virtual void SetClearColor(float red, float green, float blue, float alpha=1.0f);
		virtual bool BeginRender();
		virtual void EndRender();
		virtual void Render();
		virtual void SetRenderFillMode(RenderMode_t mode);
        virtual Shader* CreateShader(const string& vertexFilename, const string& fragmentFilename);
        virtual Texture2D* CreateTexture2D() const;

	private:
		RenderContextOpenGL*	renderContext_;	/**< The render context to create for OpenGL */

		map<const Texture2D*, pair<size_t, size_t>>	textures_;		/**< Texture mapped to the API representation of the texture */

		virtual void FillDeviceCapabilities();
};

}

#endif
