#ifndef SKETCH_3D_RENDER_CONTEXT_H
#define SKETCH_3D_RENDER_CONTEXT_H

#include "system/Window.h"

namespace Sketch3D {

/**
 * @class RenderContext
 * This class provides an interface for creating a render context depending on
 * the API used (Direct3D, OpenGL) and the OS the user is using.
 */
class RenderContext {
	public:
        /**
         * Constructor. Initialize the context from a window
         * @param window The window for which the context will be created
         */
                        RenderContext(const Window& window) : window_(window) {}

		/**
		 * Initialize the context.
		 * @return true if the intiailizing was correctly done, false otherwise
		 */
		virtual bool	Initialize() = 0;

		/**
		 * Swap the buffers from the internal device context
		 */
		virtual void	SwapBuffers() = 0;

    protected:
        const Window&   window_;    /**< The window for which the context will be created */
};

}

#endif
