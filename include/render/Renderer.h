#ifndef SKETCH_3D_RENDERER_H
#define SKETCH_3D_RENDERER_H

#include "ResourceManager.h"

#include <string>
using namespace std;

namespace Sketch3D {

/**
 * @class Renderer
 * This class provides an interface to used the underlying render system based
 * on its implementation (OpenGL or Direct3D). It is through this class that
 * the user can draw things on the screen and modify certain parameters of the
 * rendering context
 */
class Renderer {
	public:
		/**
		 * Destructor
		 */
						   ~Renderer();

		static Renderer*	getInstance();

		/**
		 * Initialize the renderer
		 * @param name The window name
		 * @param width The width of the window
		 * @param height The height of the window
		 * @param windowed Is the window in fullscreen or not?
		 * @return true if the initialization went correctly
		 */
		bool				initialize(const string& name, unsigned int width,
									   unsigned int height, bool windowed);

		/**
		 * Change the clear color
		 * @param red The red component of the color
		 * @param green The green component of the color
		 * @param blue The blue component of the color
		 */
		void				setClearColor(float red, float green, float blue);

		/**
		 * Begin the rendering process
		 * @return true if the rendering process has initialized correctly
		 */
		bool				beginRender();

		/**
		 * End the rendering process
		 */
		void				endRender();

		/**
		 * The actual rendering
		 */
		void				render();

	private:
		static Renderer		instance_;	/**< Singleton instance of this class */

		ResourceManager		resourceManager_;	/**< The resource manager of the renderer */

		/**
		 * Constructor
		 */
							Renderer();

		/**
		 * Copy-constructor - here only to disallow copy
		 */
							Renderer(const Renderer& src);

		/**
		 * Assignment operator - here only to disallow assignment
		 */
							Renderer& operator=(const Renderer& rhs);
};

}

#endif