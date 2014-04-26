#ifndef SKETCH_3D_RENDER_SYSTEM_H
#define SKETCH_3D_RENDER_SYSTEM_H

#include "system/Platform.h"
#include "system/Window.h"
#include "system/WindowHandle.h"

namespace Sketch3D {

/**
 * @interface RenderSystem
 * This class serves as an interface for the underlying implementation of the
 * rendering systems, such as OpenGL and Direct3D.
 *
 * This interface is basically the context of the API.
 */
class RenderSystem {
	public:
		/**
		 * Constructor. Initializes the underlying API.
         * @param window The window for which we want to create a context for
		 */
                        RenderSystem(const Window& window);

		/**
		 * Destructor. Free the underlying API
		 */
		virtual		   ~RenderSystem() {}

		/**
		 * Initialize the rendering system
		 * @return true if the system was correctly initialized, false otherwise
		 */
		virtual bool	Initialize()=0;

		/**
		 * Change the clear color
		 * @param red The red component
		 * @param green The green component
		 * @param blue The blue component
		 * @param alpha The alpha component
		 */
		virtual void	SetClearColor(float red, float green, float blue, float alpha=1.0f) = 0;

		/**
		 * Starts the rendering process.
		 * @return true if the rendering process was correctly started, false otherwise
		 */
		virtual bool	BeginRender() = 0;

		/**
		 * Ends the rendering process
		 */
		virtual void	EndRender() = 0;

		/**
		 * The actual rendering process
		 */
		virtual void	Render() = 0;

	protected:
        Window          window_;        /**< The window */
		WindowHandle	windowHandle_;	/**< The window's handle */
		unsigned int	width_;			/**< The width of the window */
		unsigned int	height_;		/**< The height of the window */
		bool			windowed_;		/**< IS the window in windowed mode ? */
};

}

#endif
