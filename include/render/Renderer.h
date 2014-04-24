#ifndef SKETCH_3D_RENDERER_H
#define SKETCH_3D_RENDERER_H

#include "math/Vector3.h"
#include "render/ResourceManager.h"

#include <string>
using namespace std;

namespace Sketch3D {

// Forward class declaration
class RenderSystem;
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
		 * @param renderSystem The render system to use
		 * @param window The window for which to create a rendering context
		 * @return true if the initialization went correctly
		 */
		bool				initialize(RenderSystem_t renderSystem,
									   const Window& window);

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

		/**
		 * Set an orthogonal projection. This replace the current projection matrix
		 * @param left The left position of the viewing volume
		 * @param right The right position of the viewing volume
		 * @param bottom The bottom position of the viewing volume
		 * @param top The top position of the viewing volume
		 * @param near The near position of the viewing volume
		 * @param far The far position of the viewing volume
		 */
		void				orthoProjection(float left, float right, float bottom,
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
		void				perspectiveProjection(float left, float right, float bottom,
												  float top, float near, float far);

		/**
		 * Set a new camera position, looking direction and up vector
		 * @param position The position of the camera
		 * @param direction The direction of the camera
		 * @param up The up vector to use for the camera. UP by default
		 */
		void				cameraLookAt(const Vector3& position,
										 const Vector3& direction,
										 const Vector3& up=Vector3::UP);

	private:
		static Renderer		instance_;	/**< Singleton instance of this class */

		ResourceManager		resourceManager_;	/**< The resource manager of the renderer */
		RenderSystem*		renderSystem_;		/**< The render system that is currently being used */

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