#ifndef SKETCH_3D_RENDER_CONTEXT_OPENGL_UNIX_H
#define SKETCH_3D_RENDER_CONTEXT_OPENGL_UNIX_H

#include "render/OpenGL/RenderContextOpenGL.h"

#include <X11/Xlib.h>

#include "render/OpenGL/gl/glew.h"
typedef int int32_t;
#include "render/OpenGL/gl/glx.h"

namespace Sketch3D {

/**
 * @class RenderContextOpenGLUnix
 * This class creates an OpenGL rendering context for the Unix OS
 */
class RenderContextOpenGLUnix : public RenderContextOpenGL {
    public:
        /**
         * Constructor
         * @param window The window for which the context will be created
         */
                        RenderContextOpenGLUnix(const Window& window);

        /**
         * Destructr. Free the render context
         */
        virtual        ~RenderContextOpenGLUnix();

        virtual bool    Initialize();
        virtual void    SwapBuffers();
          
    private:
        ::Window        xWindow_;  /**< X11 structure defining the window */
        ::Display*      display_;   /**< Pointer to the display */
        GLXContext      renderContext_; /**< The render context to create */
};

}

#endif
