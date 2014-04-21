#include "RenderSystemOpenGL.h"

#if PLATFORM == PLATFORM_WIN32
#include <Windows.h>
#else
#endif

#include <gl/GL.h>
#include <gl/GLU.h>

namespace Sketch3D {
RenderSystemOpenGL::RenderSystemOpenGL(sf::Window& window) : RenderSystem(window) {
}

RenderSystemOpenGL::~RenderSystemOpenGL() {
#if PLATFORM == PLATFORM_WIN32
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(renderContext_);
	ReleaseDC(window_.getSystemHandle(), deviceContext_);
#else if PLATFORM == PLATFORM_LINUX
#endif
}

bool RenderSystemOpenGL::initialize() {
#if PLATFORM == PLATFORM_WIN32
	PIXELFORMATDESCRIPTOR pixelFormat;
	
	deviceContext_ = GetDC(window_.getSystemHandle());
	if (!deviceContext_) {
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
		return false;
	}

	if (!wglMakeCurrent(deviceContext_, renderContext_)) {
		return false;
	}
#else if PLATFORM == PLATFORM_LINUX
#endif

	// Some initial values
	sf::Vector2u size = window_.getSize();
	glViewport(0, 0, size.x, size.y);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, size.x / size.y, 1.0f, 1000.0f);

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
	window_.display();
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