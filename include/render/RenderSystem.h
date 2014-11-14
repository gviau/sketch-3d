#ifndef SKETCH_3D_RENDER_SYSTEM_H
#define SKETCH_3D_RENDER_SYSTEM_H

#include "render/Renderer.h"
#include "math/Vector3.h"

#include "system/Platform.h"
#include "system/Window.h"
#include "system/WindowHandle.h"

#include <map>
#include <vector>
using namespace std;

namespace Sketch3D {

// Forward declaration
class Matrix4x4;
class RenderTexture;
class Shader;
class Texture2D;
class Texture3D;
class Vector2;

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
		virtual void					SetClearColor(float red, float green, float blue, float alpha=1.0f) const = 0;

		/**
		 * Clears the current framebuffer
         * @param buffer Which buffers to clear
		 */
		virtual void				    Clear(int buffer) const = 0;

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
		virtual void					SetRenderFillMode(RenderMode_t mode) const = 0;

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
        virtual void                    EnableDepthTest(bool val) const = 0;

        /**
         * Enable or disable depth writing. This will only work if depth testing is enabled
         * @param val Enabled if true, disabled otherwise
         */
        virtual void                    EnableDepthWrite(bool val) const = 0;

        /**
         * Sets the depth compare function to use
         * @param comparison The comparison function to use
         */
        virtual void                    SetDepthComparisonFunc(DepthFunc_t comparison) const = 0;

        /**
         * Select the culling method
         * @param cullingMethod The culling method to use
         */
        virtual void                    SetCullingMethod(CullingMethod_t cullingMethod) const = 0;

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
         * Create an empty texture
         * @return A pointer to a texture
         */
        virtual Texture3D*              CreateTexture3D() const = 0;

        /**
         * Create a render texture
         * @param width The width of the render texture
         * @param height The height of the render texture
         * @param format The format of the render texture
         * @return A pointer to a render texture
         */
        virtual RenderTexture*          CreateRenderTexture(unsigned int width, unsigned int height, TextureFormat_t format) const = 0;

        /**
         * Binds the render system frame buffer that is used to draw on the screen
         */
        virtual void                    BindScreenBuffer() const = 0;

        /**
         * Enable blending
         * @param val if true, enables blending, deactivate it otherwise
         */
        virtual void                    EnableBlending(bool val) const = 0;

        /**
         * Specifies the blending equation to use
         */
        virtual void                    SetBlendingEquation(BlendingEquation_t equation) const = 0;

        /**
         * Specifies the blending factor to use
         * @param srcFactor The factor to use for the source pixel (the pixel being processed)
         * @param dstFactor the factor to use for the destination pixel (the pixel already present in the framebuffer)
         */
        virtual void                    SetBlendingFactor(BlendingFactor_t srcFactor, BlendingFactor_t dstFactor) const = 0;

        /**
         * Convert a point in screen space into world space. This function uses the current perspective and view matrix
         * @param inversedViewProjection The inverse of the view-projection matrix
         * @param screenPoint A vector representing the point in screen space
         * @return A vector representing the point in world space
         */
        Vector3                         ScreenToWorldPoint(Matrix4x4& inversedViewProjection, const Vector2& point) const;

        size_t                          GetWidth() const { return width_; }
        size_t                          GetHeight() const { return height_; }
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
