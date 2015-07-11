#ifndef SKETCH_3D_RENDER_CONTEXT_H
#define SKETCH_3D_RENDER_CONTEXT_H

#include "render/Renderer_Common.h"
#include "render/RenderDevice_Common.h"

#include "system/Platform.h"

#include <memory>
#include <vector>
using namespace std;

namespace Sketch3D {

// Forward declaration
class Window;

/**
 * @class RenderContext
 * This class provides an interface for creating a render context depending on
 * the API used (Direct3D, OpenGL) and the OS the user is using.
 */
class SKETCH_3D_API RenderContext {
	public:
		/**
		 * Initialize the context.
		 * @param window The window for which the context will be created
         * @param renderParameters The rendering parameters to use when creating the render context
		 * @return true if the intiailizing was correctly done, false otherwise
		 */
        virtual bool	        Initialize(Window& window, const RenderParameters_t& renderParameters) = 0;

		/**
		 * Swap the buffers from the internal device context
		 */
		virtual void	        SwapBuffers() = 0;

    protected:
        vector<DisplayMode_t>   supportedDisplayModes_;   /**< List of supported display modes by the display adapter */

        /**
         * Queries the display adapter about its supported formats. This will populate the
         * supportedDisplayFormats_ list
         */
        virtual void            QueryAdapterSupportedDisplayFormats() = 0;

        /**
         * Check if the provided render parameters are supported by the display adapter
         * @param renderParameters The render parameters to check
         * @return true if they are supported, false otherwise
         */
        bool                    AreRenderParametersSupported(const RenderParameters_t& renderParameters) const;
};

// Helper to create the proper render context
shared_ptr<RenderContext> SKETCH_3D_API CreateRenderContext(RenderSystem_t renderSystem);

}

#endif
