#ifndef SKETCH_3D_RENDER_SYSTEM_H
#define SKETCH_3D_RENDER_SYSTEM_H

#include "system/Platform.h"
#include "system/Window.h"
#include "system/WindowHandle.h"

#include <map>
#include <vector>
using namespace std;

namespace Sketch3D {

// Forward declaration
class ShaderOpenGL;
class Texture2D;
enum RenderMode_t;

/**
 * @interface RenderSystem
 * This class serves as an interface for the underlying implementation of the
 * rendering systems, such as OpenGL and Direct3D.
 *
 * This interface is basically the context of the API.
 */
class RenderSystem {
		friend class ShaderOpenGL;

	public:
		/**
		 * Constructor. Initializes the underlying API.
         * @param window The window for which we want to create a context for
		 */
										RenderSystem(Window& window);

		/**
		 * Destructor. Free the underlying API
		 */
		virtual						   ~RenderSystem() {}

		/**
		 * Initialize the rendering system
		 * @return true if the system was correctly initialized, false otherwise
		 */
		virtual bool					Initialize()=0;

		/**
		 * Change the clear color
		 * @param red The red component
		 * @param green The green component
		 * @param blue The blue component
		 * @param alpha The alpha component
		 */
		virtual void					SetClearColor(float red, float green, float blue, float alpha=1.0f) = 0;

		/**
		 * Starts the rendering process.
		 * @return true if the rendering process was correctly started, false otherwise
		 */
		virtual bool					BeginRender() = 0;

		/**
		 * Ends the rendering process
		 */
		virtual void					EndRender() = 0;

		/**
		 * The actual rendering process
		 */
		virtual void					Render() = 0;

		/**
		 * Set the renderer's fill mode
		 * @param mode The mode to use for rendering the geometry
		 */
		virtual void					SetRenderFillMode(RenderMode_t mode) = 0;

		/**
		 * Create a texture using the underlying API
		 * @param texture The texture object to use for the internal
		 * representation.
		 */
		virtual void					CreateTexture(Texture2D*& texture) = 0;

		/**
		 * Enable the specified texture. There can be up to 8 enabled textures
		 * at once. If a texture is set using an index already in use, then
		 * the new texture will be used instead of the old one.
		 * @param index The texture index to use. The index has to be in
		 * [0, maxActiveTextures_], otherwise the function will silently fail.
		 * @param texture The texture to set.
         * @return The texture unit used
		 */
		virtual int					    EnableTexture(const Texture2D* texture) = 0;

		int								GetMaxActiveTextures() const { return maxActiveTextures_; }

	protected:
        Window&							window_;        /**< The window */
		WindowHandle					windowHandle_;	/**< The window's handle */
		unsigned int					width_;			/**< The width of the window */
		unsigned int					height_;		/**< The height of the window */
		bool							windowed_;		/**< Is the window in windowed mode ? */

		// Device capabilities
		int								maxActiveTextures_; /**< Maximum number of active textures supported by the GPU */

		/**
		 * Fill in the device capabilities
		 */
		virtual void					FillDeviceCapabilities() = 0;
};

}

#endif
