#include "render/OpenGL/RenderSystemOpenGL.h"

#include "render/OpenGL/gl/glew.h"
#include "render/OpenGL/gl/gl.h"

#include "render/Renderer.h"
#include "render/Texture2D.h"

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

	for (int i = 0; i < maxActiveTextures_; i++) {
		activeTextures_.push_back(-1);
	}

	// Some initial values
	glViewport(0, 0, width_, height_);

	Renderer::GetInstance()->PerspectiveProjection(45.0f, (float)width_ / (float)height_, 1.0f, 1000.0f);

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
	static float angle = 0.0f;
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	angle += 0.02f;
}

void RenderSystemOpenGL::SetRenderFillMode(RenderMode_t mode) {
	switch (mode) {
		case RENDER_MODE_FILL:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case RENDER_MODE_WIREFRAME:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case RENDER_MODE_POINT:
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			break;
	}
}

void RenderSystemOpenGL::CreateTexture(const Texture2D& texture) {
	GLuint tex;
	glGenTextures(1, &tex);
	
	// We have to replace our currently bound texture on the last slot (the
	// one less likely to be used) so we can bind this new texture for its
	// creation.
	glActiveTexture(GL_TEXTURE0 + maxActiveTextures_ - 1);
	glBindTexture(GL_TEXTURE_2D, tex);

	int filter, wrap, format;
	switch (texture.filterMode_) {
		case FILTER_MODE_POINT:
			filter = GL_NEAREST;
			break;

		case FILTER_MODE_BILINEAR:
			filter = GL_LINEAR;
			break;

			// TODO
		case FILTER_MODE_TRILINEAR:
		default:
			filter = GL_LINEAR;
	}

	switch (texture.wrapMode_) {
		case WRAP_MODE_CLAMP:
			wrap = GL_CLAMP;
			break;

		case WRAP_MODE_REPEAT:
			wrap = GL_REPEAT;
			break;
	}

	switch (texture.format_) {
		case TEXTURE_FORMAT_RGB24:
			format = GL_RGB;
			break;

		case TEXTURE_FORMAT_RGBA32:
			format = GL_RGBA;
			break;

	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width_, texture.height_, 0,
				 format, GL_UNSIGNED_BYTE, &texture.data_[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

	textures_[&texture] = tex;

	if (activeTextures_[maxActiveTextures_ - 1] != -1) {
		glBindTexture(GL_TEXTURE_2D, activeTextures_[maxActiveTextures_ - 1]);
	}
}

void RenderSystemOpenGL::EnableTexture(unsigned int index, const Texture2D& texture) {
	if (index >= (unsigned int)maxActiveTextures_) {
		return;
	}

	map<const Texture2D*, size_t>::iterator it = textures_.find(&texture);
	if (it != textures_.end()) {
		return;
	}

	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, it->second);
}

void RenderSystemOpenGL::FillDeviceCapabilities() {
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxActiveTextures_);
}

}
