#include "render/OpenGL/RenderSystemOpenGL.h"

#include "render/OpenGL/gl/glew.h"
#include "render/OpenGL/gl/gl.h"

#include "render/Renderer.h"
#include "render/OpenGL/ShaderOpenGL.h"
#include "render/OpenGL/Texture2DOpenGL.h"

#include "system/Logger.h"
#include "system/Window.h"

#if PLATFORM == PLATFORM_WIN32
#include "render/OpenGL/Win32/RenderContextOpenGLWin32.h"
#elif PLATFORM == PLATFORM_LINUX
#include "render/OpenGL/Unix/RenderContextOpenGLUnix.h"
#endif

namespace Sketch3D {

RenderSystemOpenGL::RenderSystemOpenGL(Window& window) : RenderSystem(window),
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
	renderContext_ = new RenderContextOpenGLWin32();
#elif PLATFORM == PLATFORM_LINUX
    renderContext_ = new RenderContextOpenGLUnix();
#endif

	if (!renderContext_->Initialize(window_)) {
		Logger::GetInstance()->Error("Couldn't create OpenGL context");
		return false;
	}

	FillDeviceCapabilities();

	// Some initial values
	glViewport(0, 0, width_, height_);

	Renderer::GetInstance()->PerspectiveProjection(45.0f, (float)width_ / (float)height_, 1.0f, 1000.0f);

    //glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
    SetRenderFillMode(RenderMode_t::RENDER_MODE_FILL);

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
	static float angle = 0.0f;
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	angle += 0.02f;
}

void RenderSystemOpenGL::SetRenderFillMode(RenderMode_t mode) {
	switch (mode) {
		case RENDER_MODE_FILL:
			glPolygonMode(GL_FRONT, GL_FILL);
			break;
		case RENDER_MODE_WIREFRAME:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case RENDER_MODE_POINT:
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			break;
	}
}

Shader* RenderSystemOpenGL::CreateShader(const string& vertexFilename, const string& fragmentFilename) {
    shaders_.push_back(new ShaderOpenGL(vertexFilename, fragmentFilename));
    return shaders_[shaders_.size() - 1];
}

Texture2D* RenderSystemOpenGL::CreateTexture2D() const {
    return new Texture2DOpenGL;
}

void RenderSystemOpenGL::FillDeviceCapabilities() {
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxActiveTextures_);
}

}
