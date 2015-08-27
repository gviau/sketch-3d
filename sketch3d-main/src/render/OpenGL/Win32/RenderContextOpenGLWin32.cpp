#include "render/OpenGL/Win32/RenderContextOpenGLWin32.h"

#include "system/Logger.h"
#include "system/Window.h"

#include "render/OpenGL/GL/glew.h"
#include "render/OpenGL/GL/wglext.h"

namespace Sketch3D {

RenderContextOpenGLWin32::RenderContextOpenGLWin32() : renderContext_(NULL)
{
}

RenderContextOpenGLWin32::~RenderContextOpenGLWin32()
{
	if (renderContext_)
    {
		if (wglGetCurrentContext() == renderContext_)
        {
			wglMakeCurrent(NULL, NULL);
		}

		wglDeleteContext(renderContext_);
	}

    ChangeDisplaySettings(NULL, 0);
}

bool RenderContextOpenGLWin32::Initialize(Window& window, const RenderParameters_t& renderParameters)
{
	Logger::GetInstance()->Debug("Initializing OpenGL context");

    size_t colorBits, redBits, greenBits, blueBits, depthBits;
    size_t alphaBits = 0;
    size_t stencilBits = 0;
    switch (renderParameters.displayFormat)
    {
    case DisplayFormat_t::A1R5G5B5:
        colorBits = 16;
        alphaBits = 1;
        redBits = blueBits = 5;
        greenBits = 6;
        break;

    case DisplayFormat_t::A2R10G10B10:
        colorBits = 24;
        alphaBits = 2;
        redBits = greenBits = blueBits = 10;
        break;

    case DisplayFormat_t::A8R8G8B8:
        colorBits = 24;
        alphaBits = 8;
        redBits = greenBits = blueBits = 8;
        break;

    case DisplayFormat_t::R5G6B5:
        colorBits = 16;
        redBits = blueBits = 5;
        greenBits = 6;
        break;

    case DisplayFormat_t::X8R8G8B8:
        colorBits = 24;
        redBits = greenBits = blueBits = 8;
        break;
    }

    switch (renderParameters.depthStencilBits)
    {
    case DepthStencilBits_t::D16:
        depthBits = 16;
        break;

    case DepthStencilBits_t::D24S8:
        depthBits = 24;
        stencilBits = 8;
        break;
    case DepthStencilBits_t::D32:
        depthBits = 32;
        break;
    }

    if (!window.IsWindowed())
    {
        // From http://www.falloutsoftware.com/tutorials/gl/gl2.htm
        DEVMODE dmode;
 
        memset(&dmode, 0, sizeof(DEVMODE));
        dmode.dmSize=sizeof(DEVMODE);
        dmode.dmPelsWidth = renderParameters.width;
        dmode.dmPelsHeight = renderParameters.height;
        dmode.dmBitsPerPel = (colorBits == 24) ? 32 : colorBits;
        dmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
 
        // Change resolution, if possible
        if (ChangeDisplaySettings(&dmode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
        {
            Logger::GetInstance()->Warning("Couldn't set window to fullscreen mode");
        }

        // Make the window flags compatible with fullscreen mode
        SetWindowLongW(window.GetHandle(), GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
        SetWindowLongW(window.GetHandle(), GWL_EXSTYLE, WS_EX_APPWINDOW);
        SetWindowPos(window.GetHandle(), NULL, 0, 0, renderParameters.width, renderParameters.height, SWP_FRAMECHANGED);
        ShowWindow(window.GetHandle(), SW_SHOW);
    }

    deviceContext_ = GetDC(reinterpret_cast<HWND>(window.GetHandle()));
    if (!deviceContext_)
    {
        Logger::GetInstance()->Error("Couldn't retrieve device context");
        return false;
    }

	PIXELFORMATDESCRIPTOR pixelFormat;
	ZeroMemory(&pixelFormat, sizeof(pixelFormat));
	pixelFormat.nSize = sizeof(pixelFormat);
	pixelFormat.nVersion = 1;
	pixelFormat.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
						  PFD_DOUBLEBUFFER;
	pixelFormat.iPixelType = PFD_TYPE_RGBA;
    pixelFormat.cColorBits = colorBits;
    pixelFormat.cDepthBits = depthBits;
    pixelFormat.cAlphaBits = alphaBits;
    pixelFormat.cRedBits = redBits;
    pixelFormat.cGreenBits = greenBits;
    pixelFormat.cBlueBits = blueBits;
    pixelFormat.cStencilBits = stencilBits;
	pixelFormat.iLayerType = PFD_MAIN_PLANE;

	int format = ChoosePixelFormat(deviceContext_, &pixelFormat);
	if (format == 0)
    {
		Logger::GetInstance()->Error("Failed to create a suitable pixel format for device context");
        return false;
	}

	if (!SetPixelFormat(deviceContext_, format, &pixelFormat))
    {
		Logger::GetInstance()->Error("Couldn't set the pixel format");
		return false;
	}
	
	// Create a dummy context, because we are going to create a context using
	// an extension function
	HGLRC dummyRenderContext = wglCreateContext(deviceContext_);
	wglMakeCurrent(deviceContext_, dummyRenderContext);

	int attributes[] =
    {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 2,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		0, 0
	};

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
	renderContext_ = wglCreateContextAttribsARB(deviceContext_, NULL, attributes);
	if (!renderContext_)
    {
        wglDeleteContext(dummyRenderContext);
		Logger::GetInstance()->Error("Couldn't create dummy render context");
		return false;
	}

    if (!wglDeleteContext(dummyRenderContext))
    {
        Logger::GetInstance()->Error("Couldn't delete dummy context");
        return false;
    }

	if (!wglMakeCurrent(deviceContext_, renderContext_))
    {
		Logger::GetInstance()->Error("Couldn't set the new rendering context");
		return false;
	}
	
	if (GLEW_OK != glewInit())
    {
		Logger::GetInstance()->Error("Couldn't initialize GLEW library");
		return false;
	}

	Logger::GetInstance()->Debug("Render context created");
	return true;
}

void RenderContextOpenGLWin32::SwapBuffers()
{
	::SwapBuffers(deviceContext_);
}

void RenderContextOpenGLWin32::QueryAdapterSupportedDisplayFormats()
{

}

}