#ifndef SKETCH_3D_RENDER_CONTEXT_OPENGL_H
#define SKETCH_3D_RENDER_CONTEXT_OPENGL_H

#include "render/RenderContext.h"

namespace Sketch3D {

/**
 * @class RenderContextOpenGL
 * This class acts as a base class for the OpenGL context creation. Derived
 * class will implement the code depending what OS is the user using.
 */
class RenderContextOpenGL : public RenderContext {
	public:
        RenderContextOpenGL(const Window& window) : RenderContext(window) {}
		virtual bool Initialize() = 0;
		virtual void SwapBuffers() = 0;
};

}

#endif
