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
class RenderTexture;
class Shader;
class Texture2D;
enum RenderMode_t;
enum TextureFormat_t;

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
										RenderSystem(Window& window);

		/**
		 * Destructor. Free the underlying API
		 */
		virtual						   ~RenderSystem();

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
         * Sets the camera viewport dimension
         * @param x The left position of the viewport
         * @param y The upper position of the viewport
         * @param width The width of the viewport
         * @param height The height of the viewport
         */
        virtual void                    SetViewport(size_t x, size_t y, size_t width, size_t height) = 0;

        /**
         * Enable or disable depth testing
         * @param val Enabled if true, disabled otherwise
         */
        virtual void                    EnableDepthTest(bool val) = 0;

        /**
         * Create an empty shader
         * @param vertexFilename The vertex shader filename
         * @param fragmentFilename The fragment shader filename
         * @param vertexInputs List of vertex inputs in order
         * @return A pointer to a shader
         */
        virtual Shader*                 CreateShader(const string& vertexFilename, const string& fragmentFilename, const vector<string>& vertexInputs) = 0;

        /**
         * Create an empty texture
         * @return A pointer to a texture
         */
        virtual Texture2D*              CreateTexture2D() const = 0;

        /**
         * Create a render texture
         * @param width The width of the render texture
         * @param height The height of the render texture
         * @param format The format of the render texture
         * @return A pointer to a render texture
         */
        virtual RenderTexture*          CreateRenderTexture(unsigned int width, unsigned int height, TextureFormat_t format) const = 0;

		int								GetMaxActiveTextures() const { return maxActiveTextures_; }

	protected:
        Window&							window_;        /**< The window */
		WindowHandle					windowHandle_;	/**< The window's handle */
		unsigned int					width_;			/**< The width of the window */
		unsigned int					height_;		/**< The height of the window */
		bool							windowed_;		/**< Is the window in windowed mode ? */

        vector<Shader*>                 shaders_;       /**< List of all shaders created */

		// Device capabilities
		int								maxActiveTextures_; /**< Maximum number of active textures supported by the GPU */

		/**
		 * Fill in the device capabilities
		 */
		virtual void					FillDeviceCapabilities() = 0;
};

}

#endif
