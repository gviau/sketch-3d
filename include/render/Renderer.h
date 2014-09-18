#ifndef SKETCH_3D_RENDERER_H
#define SKETCH_3D_RENDERER_H

#include "math/Matrix4x4.h"
#include "math/Vector3.h"

#include "render/ResourceManager.h"
#include "render/SceneTree.h"

#include <string>
using namespace std;

namespace Sketch3D {

// Forward class declaration
class RenderSystem;
class RenderTexture;
class Shader;
class Window;

/**
 * @enum RenderSystem_t
 * Enum that give the possible render system that one can use
 */
enum RenderSystem_t {
	RENDER_SYSTEM_OPENGL,
	RENDER_SYSTEM_DIRECT3D9
};

/**
 * @enum RenderMode
 * Enum that give the possible rendering mode
 */
enum RenderMode_t {
	RENDER_MODE_FILL,
	RENDER_MODE_WIREFRAME,
	RENDER_MODE_POINT
};

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

		static Renderer*	GetInstance();

		/**
		 * Initialize the renderer
		 * @param renderSystem The render system to use
		 * @param window The window for which to create a rendering context
		 * @return true if the initialization went correctly
		 */
		bool				Initialize(RenderSystem_t renderSystem,
									   Window& window);

		/**
		 * Change the clear color
		 * @param red The red component of the color
		 * @param green The green component of the color
		 * @param blue The blue component of the color
		 * @param alpha The alpha component of the color
		 */
		void				SetClearColor(float red, float green, float blue, float alpha=0.0f);

		/**
		 * Begin the rendering process
		 * @return true if the rendering process has initialized correctly
		 */
		bool				BeginRender();

		/**
		 * End the rendering process
		 */
		void				EndRender();

		/**
		 * The actual rendering
		 */
		void				Render();

		/**
		 * Set an orthogonal projection. This replace the current projection matrix
		 * @param left The left position of the viewing volume
		 * @param right The right position of the viewing volume
		 * @param bottom The bottom position of the viewing volume
		 * @param top The top position of the viewing volume
		 * @param near The near position of the viewing volume
		 * @param far The far position of the viewing volume
		 */
		void				OrthoProjection(float left, float right, float bottom,
											float top, float near, float far);

		/**
		 * Set a perspective projection. This replace the current projection matrix
		 * @param left The left position of the viewing volume
		 * @param right The right position of the viewing volume
		 * @param bottom The bottom position of the viewing volume
		 * @param top The top position of the viewing volume
		 * @param near The near position of the viewing volume
		 * @param far The far position of the viewing volume
		 */
		void				PerspectiveProjection(float left, float right, float bottom,
												  float top, float near, float far);

		/**
		 * Set a perspective projection. This replace the current projection matrix
		 * @param fov The field of view in degrees in the y direction
		 * @param aspect The aspect ration
		 * @param near The near position of the viewing volume
		 * @param far The far position of the viewing volume
		 */
		void				PerspectiveProjection(float fov, float aspect,
												  float near, float far);

		/**
		 * Set a new camera position, looking direction and up vector
		 * @param position The position of the camera
		 * @param point The point to look at
		 * @param up The up vector to use for the camera. UP by default
		 */
		void				CameraLookAt(const Vector3& position,
										 const Vector3& point,
										 const Vector3& up=Vector3::UP);

		/**
		 * Set the renderer's fill mode
		 * @param mode The mode to use for rendering the geometry
		 */
		void				SetRenderFillMode(RenderMode_t mode);

        /**
         * Sets the camera viewport dimension
         * @param x The left position of the viewport
         * @param y The upper position of the viewport
         * @param width The width of the viewport
         * @param height The height of the viewport
         */
        void                SetViewport(size_t x, size_t y, size_t width, size_t height);

        /**
         * Enable or disable depth testing
         * @param val Enabled if true, disabled otherwise
         */
        void                EnableDepthTest(bool val);

        /**
         * Create a shader object
         * @param vertexFilename The vertex shader filename
         * @param fragmentFilename The fragment shader filename
         * @return A pointer to a shader
         */
        Shader*             CreateShader(const string& vertexFilename, const string& fragmentFilename) const;

        /**
         * Create a 2D texture object
         * @return A pointer to a 2D texture
         */
        Texture2D*          CreateTexture2D() const;

        /**
         * Create a render texture
         * @param width The width of the render texture
         * @param height The height of the render texture
         * @param format The format of the render texture
         * @return A pointer to a render texture
         */
        RenderTexture*      CreateRenderTexture(unsigned int width, unsigned int height, TextureFormat_t format) const;

		const Matrix4x4&	GetProjectionMatrix() const;
		const Matrix4x4&	GetViewMatrix() const;
		const Matrix4x4&	GetViewProjectionMatrix() const;

		const SceneTree&	GetSceneTree() const;
		SceneTree&			GetSceneTree();

	private:
		static Renderer		instance_;	/**< Singleton instance of this class */

		RenderSystem*		renderSystem_;		/**< The render system that is currently being used */

		Matrix4x4			projection_;		/**< The projection matrix */
		Matrix4x4			view_;				/**< The view matrix */
		Matrix4x4			viewProjection_;	/**< Pre-computed view-projection matrix */

		SceneTree			sceneTree_;			/**< The scene tree to render */

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