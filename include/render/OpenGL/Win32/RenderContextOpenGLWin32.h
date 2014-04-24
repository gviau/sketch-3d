#ifndef SKETCH_3D_RENDER_CONTEXT_OPENGL_WIN32_H
#define SKETCH_3D_RENDER_CONTEXT_OPENGL_WIN32_H

#include "render/OpenGL/RenderContextOpenGL.h"

#include <Windows.h>

namespace Sketch3D {

/**
 * @class RenderContextOpenGLWin32
 * This class creates an OpenGL rendering context for the Windows OS
 */
class RenderContextOpenGLWin32 : public RenderContextOpenGL {
	public:
		/**
		 * Constructor
		 * @param deviceContext The device of the window
		 */
						RenderContextOpenGLWin32(HDC deviceContext);

		/**
		 * Destructor. Free the render context
		 */
		virtual		   ~RenderContextOpenGLWin32();

		virtual bool	Initialize();
		virtual void	SwapBuffers();

	private:
		HDC				deviceContext_;		/**< Window's device context */
		HGLRC			renderContext_;		/**< The render context to initialize */
};

}

#endif