#ifndef SKETCH_3D_RENDER_CONTEXT_H
#define SKETCH_3D_RENDER_CONTEXT_H

namespace Sketch3D {

// Forward declaration
class Window;

/**
 * @class RenderContext
 * This class provides an interface for creating a render context depending on
 * the API used (Direct3D, OpenGL) and the OS the user is using.
 */
class RenderContext {
	public:
		/**
		 * Initialize the context.
		 * @param window The window for which the context will be created
		 * @return true if the intiailizing was correctly done, false otherwise
		 */
		virtual bool	Initialize(Window& window) = 0;

		/**
		 * Swap the buffers from the internal device context
		 */
		virtual void	SwapBuffers() = 0;
};

}

#endif
