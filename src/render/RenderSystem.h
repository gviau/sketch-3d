#ifndef SKETCH_3D_RENDER_SYSTEM_H2
#define SKETCH_3D_RENDER_SYSTEM_H

#include <SFML/Window.hpp>

#if PLATFORM == PLATFORM_WIN32
#	include <Windows.h>
#	define WINDOW_HANDLE HWND
#	define DEVICE_CONTEXT HDC
#	define RENDER_CONTEXT HGLRC
#else if PLATFORM == PLATFORM_LINUX
#	define WINDOW_HANDLE
#	define DEVICE_CONTEXT
#	define RENDER_CONTEXT
#endif

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
		 * @param window A SFML window to initialize the context
		 */
						RenderSystem(sf::Window& window) : window_(window) {}

		/**
		 * Destructor. Free the underlying API
		 */
		virtual		   ~RenderSystem() {}

		/**
		 * Initialize the rendering system
		 * @return true if the system was correctly initialized, false otherwise
		 */
		virtual bool	initialize()=0;

		/**
		 * Change the clear color
		 * @param red The red component
		 * @param green The green component
		 * @param blue The blue component
		 * @param alpha The alpha component
		 */
		virtual void	setClearColor(float red, float green, float blue, float alpha=1.0f) = 0;

		/**
		 * Starts the rendering process.
		 * @return true if the rendering process was correctly started, false otherwise
		 */
		virtual bool	beginRender() = 0;

		/**
		 * Ends the rendering process
		 */
		virtual void	endRender() = 0;

		/**
		 * The actual rendering process
		 */
		virtual void	render() = 0;

	protected:
		sf::Window&		window_;

		DEVICE_CONTEXT	deviceContext_;
		RENDER_CONTEXT	renderContext_;
};

}

#endif