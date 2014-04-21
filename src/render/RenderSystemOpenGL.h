#ifndef SKETCH_3D_RENDER_SYSTEM_OPENGL_H
#define SKETCH_3D_RENDER_SYSTEM_OPENGL_H

#include "RenderSystem.h"

namespace Sketch3D {

/**
 * @class RenderSystem
 * OpenGL implementation of the render system
 */
class RenderSystemOpenGL : public RenderSystem {
	public:
		RenderSystemOpenGL(sf::Window& window);
		~RenderSystemOpenGL();
		virtual bool initialize();
		virtual void setClearColor(float red, float green, float blue, float alpha=1.0f);
		virtual bool beginRender();
		virtual void endRender();
		virtual void render();
	private:
};

}

#endif