#include "render/OpenGL/RenderSystemOpenGL.h"

#include "system/Logger.h"
#include "system/Window.h"

#include "render/OpenGL/gl/glew.h"
#include "render/OpenGL/gl/gl.h"

#if PLATFORM == PLATFORM_WIN32
#include "render/OpenGL/Win32/RenderContextOpenGLWin32.h"
#elif PLATFORM == PLATFORM_LINUX
#include "render/OpenGL/Unix/RenderContextOpenGLUnix.h"
#endif

namespace Sketch3D {

RenderSystemOpenGL::RenderSystemOpenGL(const Window& window) : RenderSystem(window),
                                                               renderContext_(NULL)
{
	Logger::GetInstance()->Info("Current rendering API: OpenGL");
}

RenderSystemOpenGL::~RenderSystemOpenGL() {
	Logger::GetInstance()->Info("Shutdown OpenGL");
	delete renderContext_;
}

bool RenderSystemOpenGL::Initialize() {
	Logger::GetInstance()->Info("Initializing OpenGL...");

#if PLATFORM == PLATFORM_WIN32
	renderContext_ = new RenderContextOpenGLWin32(window_);
#elif PLATFORM == PLATFORM_LINUX
    renderContext_ = new RenderContextOpenGLUnix(window_);
#endif

	if (!renderContext_->Initialize()) {
		Logger::GetInstance()->Error("Couldn't create OpenGL context");
		return false;
	}

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

void RenderSystemOpenGL::SetClearColor(float red, float green, float blue, float alpha) {
	glClearColor(red, green, blue, alpha);
}

bool RenderSystemOpenGL::BeginRender() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return true;
}

void RenderSystemOpenGL::EndRender() {
	renderContext_->SwapBuffers();
}

void RenderSystemOpenGL::Render() {
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
