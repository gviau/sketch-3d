#include "render/OpenGL/Win32/RenderContextOpenGLWin32.h"

#include "system/Logger.h"

#include <gl/GL.h>
#include <gl/GLU.h>
#include "render/OpenGL/gl/wglext.h"

namespace Sketch3D {

RenderContextOpenGLWin32::RenderContextOpenGLWin32(HDC deviceContext) : deviceContext_(deviceContext),
																		renderContext_(NULL)
{
}

RenderContextOpenGLWin32::~RenderContextOpenGLWin32() {
	if (renderContext_) {
		if (wglGetCurrentContext() == renderContext_) {
			wglMakeCurrent(NULL, NULL);
		}

		wglDeleteContext(renderContext_);
	}
}

bool RenderContextOpenGLWin32::Initialize() {
	Logger::GetInstance()->Debug("Initializing OpenGL context");

	PIXELFORMATDESCRIPTOR pixelFormat;
	ZeroMemory(&pixelFormat, sizeof(pixelFormat));
	pixelFormat.nSize = sizeof(pixelFormat);
	pixelFormat.nVersion = 1;
	pixelFormat.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
						  PFD_DOUBLEBUFFER;
	pixelFormat.iPixelType = PFD_TYPE_RGBA;
	pixelFormat.cColorBits = 24;
	pixelFormat.cDepthBits = 16;
	pixelFormat.iLayerType = PFD_MAIN_PLANE;

	int format = ChoosePixelFormat(deviceContext_, &pixelFormat);
	if (format == 0) {
		Logger::GetInstance()->Error("Failed to create a suitable pixel format "
									 "for device context - cannot create OpenGL context");
		return false;
	}

	if (!SetPixelFormat(deviceContext_, format, &pixelFormat)) {
		Logger::GetInstance()->Error("Couldn't set the pixel format - cannot "
									 "create OpenGL context");
		return false;
	}
	
	// Create a dummy context, because we are going to create a context using
	// an extension function
	renderContext_ = wglCreateContext(deviceContext_);
	wglMakeCurrent(deviceContext_, renderContext_);

	int attributes[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		0, 0
	};

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
	renderContext_ = wglCreateContextAttribsARB(deviceContext_, NULL, attributes);
	if (!renderContext_) {
		Logger::GetInstance()->Error("Couldn't create render context");
		return false;
	}

	if (!wglMakeCurrent(deviceContext_, renderContext_)) {
		Logger::GetInstance()->Error("Couldn't set the new rendering context");
		return false;
	}

	Logger::GetInstance()->Debug("Render context created");
	return true;
}

void RenderContextOpenGLWin32::SwapBuffers() {
	::SwapBuffers(deviceContext_);
}

}