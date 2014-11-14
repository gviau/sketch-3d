#define GLX_GLXEXT_LEGACY
#include "render/OpenGL/Unix/RenderContextOpenGLUnix.h"

#include "system/Logger.h"
#include "system/Window.h"

#include <X11/Xutil.h>

#include "render/OpenGL/gl/glxext.h"

namespace Sketch3D {

RenderContextOpenGLUnix::RenderContextOpenGLUnix()  {
    display_ = XOpenDisplay(NULL);
}

RenderContextOpenGLUnix::~RenderContextOpenGLUnix() {
}

bool RenderContextOpenGLUnix::Initialize(Window& window) {
    Logger::GetInstance()->Debug("Initializing OpenGL context");

    xWindow_ = static_cast< ::Window>(window.GetHandle());
    XVisualInfo* visual = NULL;

    const GLubyte* name = reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB");
    PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>(glXGetProcAddress(name));
    if (!glXCreateContextAttribsARB) {
        Logger::GetInstance()->Error("Couldn't retrieve glXCreateContextAttribsARB "
                                     "function.");
        return false;
    }

    int nbConfigs = 0;
    int fbAttributes[] = {
        GLX_DEPTH_SIZE, 24,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DOUBLEBUFFER, True,
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_CONFIG_CAVEAT, GLX_NONE,
        None
    };

    GLXFBConfig* configs = glXChooseFBConfig(display_, DefaultScreen(display_),
                                             fbAttributes, &nbConfigs);
    if (!configs || !nbConfigs) {
        Logger::GetInstance()->Error("Couldn't choose FB config");
        return false;
    }

    int attributes[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 1,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
        0, 0
    };

    renderContext_ = glXCreateContextAttribsARB(display_, configs[0], NULL,
                                                true, attributes);
    if (!renderContext_) {
        Logger::GetInstance()->Error("Couldn't create render context");
        return false;
    }

    visual = glXGetVisualFromFBConfig(display_, configs[0]);

    XFree(configs);

    glXMakeCurrent(display_, xWindow_, renderContext_);

    Logger::GetInstance()->Debug("Render context created");
    return true;
}

void RenderContextOpenGLUnix::SwapBuffers() {
    glXSwapBuffers(display_, xWindow_);
}
                                                                       
}
