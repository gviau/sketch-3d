#define GLX_GLXEXT_LEGACY
#include "render/OpenGL/Unix/RenderContextOpenGLUnix.h"

#include "system/Logger.h"
#include "system/Window.h"

#include <X11/Xutil.h>

#include "render/OpenGL/gl/glew.h"
#include "render/OpenGL/gl/glxext.h"

namespace Sketch3D {

RenderContextOpenGLUnix::RenderContextOpenGLUnix()  {
    display_ = XOpenDisplay(NULL);
}

RenderContextOpenGLUnix::~RenderContextOpenGLUnix() {
}

bool RenderContextOpenGLUnix::Initialize(Window& window, const RenderParameters_t& renderParameters) {
    Logger::GetInstance()->Debug("Initializing OpenGL context");

    xWindow_ = static_cast< ::Window>(window.GetHandle());

    const GLubyte* name = reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB");
    PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>(glXGetProcAddress(name));
    if (!glXCreateContextAttribsARB) {
        Logger::GetInstance()->Error("Couldn't retrieve glXCreateContextAttribsARB "
                                     "function.");
        return false;
    }

    int colorBits, redBits, greenBits, blueBits, depthBits;
    int alphaBits = 0;
    int stencilBits = 0;
    switch (renderParameters.displayFormat) {
        case DISPLAY_FORMAT_A1R5G5B5:
            colorBits = 16;
            alphaBits = 1;
            redBits = blueBits = 5;
            greenBits = 6;
            break;

        case DISPLAY_FORMAT_A2R10G10B10:
            colorBits = 24;
            alphaBits = 2;
            redBits = greenBits = blueBits = 10;
            break;

        case DISPLAY_FORMAT_A8R8G8B8:
            colorBits = 24;
            alphaBits = 8;
            redBits = greenBits = blueBits = 8;
            break;

        case DISPLAY_FORMAT_R5G6B5:
            colorBits = 16;
            redBits = blueBits = 5;
            greenBits = 6;
            break;

        case DISPLAY_FORMAT_X1R5G5B5:
            colorBits = 16;
            redBits = greenBits = blueBits = 5;
            break;

        case DISPLAY_FORMAT_X8R8G8B8:
            colorBits = 24;
            redBits = greenBits = blueBits = 8;
            break;
    }

    switch (renderParameters.depthStencilBits) {
        case DEPTH_STENCIL_BITS_D15S1:
            depthBits = 15;
            stencilBits = 1;
            break;

        case DEPTH_STENCIL_BITS_D16:
            depthBits = 16;
            break;

        case DEPTH_STENCIL_BITS_D24S8:
            depthBits = 24;
            stencilBits = 8;
            break;

        case DEPTH_STENCIL_BITS_D24X4S4:
            depthBits = 24;
            stencilBits = 4;
            break;

        case DEPTH_STENCIL_BITS_D24X8:
            depthBits = 24;
            break;

        case DEPTH_STENCIL_BITS_D32:
            depthBits = 32;
            break;
    }

    int nbConfigs = 0;
    int fbAttributes[] = {
        GLX_DEPTH_SIZE, depthBits,
        GLX_STENCIL_SIZE, stencilBits,
        GLX_RED_SIZE, redBits,
        GLX_GREEN_SIZE, greenBits,
        GLX_BLUE_SIZE, blueBits,
        GLX_ALPHA_SIZE, alphaBits,
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
        GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
        0, 0
    };

    renderContext_ = glXCreateContextAttribsARB(display_, configs[0], NULL,
                                                true, attributes);
    if (!renderContext_) {
        Logger::GetInstance()->Error("Couldn't create render context");
        return false;
    }

    XFree(configs);

    if (!glXMakeCurrent(display_, xWindow_, renderContext_)) {
        Logger::GetInstance()->Error("Couldn't set current OpenGL context");
        return false;
    }

    if (GLEW_OK != glewInit()) {
        Logger::GetInstance()->Error("Couldn't initialize GLEW library");
        return false;
    }

    Logger::GetInstance()->Debug("Render context created");
    return true;
}

void RenderContextOpenGLUnix::SwapBuffers() {
    glXSwapBuffers(display_, xWindow_);
}

void RenderContextOpenGLUnix::QueryAdapterSupportedDisplayFormats() {
}
                                                                       
}
