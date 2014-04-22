#include "render/RenderSystemOpenGL.h"

#include "system/Logger.h"

#include <gl/GL.h>
#include <gl/GLU.h>

namespace Sketch3D {
RenderSystemOpenGL::RenderSystemOpenGL(WindowHandle windowHandle, unsigned int width,
									 unsigned int height, bool windowed) : RenderSystem(windowHandle,
																						width, height,
																						windowed)
{
	Logger::getInstance()->info("Current rendering API: OpenGL");
}

RenderSystemOpenGL::~RenderSystemOpenGL() {
	Logger::getInstance()->info("Shutdown OpenGL");

#if PLATFORM == PLATFORM_WIN32
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(renderContext_);
	ReleaseDC(windowHandle_, deviceContext_);
#else if PLATFORM == PLATFORM_LINUX
#endif
}

bool RenderSystemOpenGL::initialize() {
	Logger::getInstance()->info("Initializing OpenGL...");

#if PLATFORM == PLATFORM_WIN32
	PIXELFORMATDESCRIPTOR pixelFormat;
	
	deviceContext_ = GetDC(windowHandle_);
	if (!deviceContext_) {
		Logger::getInstance()->error("Couldn't retrieve device context");
		return false;
	}

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
	SetPixelFormat(deviceContext_, format, &pixelFormat);

	renderContext_ = wglCreateContext(deviceContext_);
	if (!renderContext_) {
		Logger::getInstance()->error("Couldn't create render context");
		return false;
	}

	if (!wglMakeCurrent(deviceContext_, renderContext_)) {
		Logger::getInstance()->error("Couldn't set the new rendering context");
		return false;
	}
#else if PLATFORM == PLATFORM_LINUX
#endif

	// Some initial values
	glViewport(0, 0, width_, height_);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, width_ / height_, 1.0f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	return true;
}

void RenderSystemOpenGL::setClearColor(float red, float green, float blue, float alpha) {
	glClearColor(red, green, blue, alpha);
}

bool RenderSystemOpenGL::beginRender() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return true;
}

void RenderSystemOpenGL::endRender() {
	SwapBuffers(deviceContext_);
}

void RenderSystemOpenGL::render() {
	glLoadIdentity();
	gluLookAt(0.0f, 1.0f, 10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	static float angle = 0.0f;
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	angle += 1.0f;

	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glEnd();
}

}