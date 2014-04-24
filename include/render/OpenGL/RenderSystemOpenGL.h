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
		RenderSystemOpenGL(WindowHandle windowHandle, unsigned int width, unsigned int height, bool windowed);
		~RenderSystemOpenGL();
		virtual bool initialize();
		virtual void setClearColor(float red, float green, float blue, float alpha=1.0f);
		virtual bool beginRender();
		virtual void endRender();
		virtual void render();

	public:
		RenderContextOpenGL*	renderContext_;	/**< The render context to create for OpenGL */
};

}

#endif