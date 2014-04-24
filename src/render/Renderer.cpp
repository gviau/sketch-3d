#include "render/Renderer.h"

#include "render/OpenGL/RenderSystemOpenGL.h"
#include "system/Logger.h"
#include "system/Window.h"

namespace Sketch3D {

Renderer Renderer::instance_;

Renderer::Renderer() : renderSystem_(NULL) {
}

Renderer::~Renderer() {
	delete renderSystem_;
}

Renderer* Renderer::getInstance() {
	return &instance_;
}

bool Renderer::initialize(RenderSystem_t renderSystem,
						  const Window& window)
{
	switch (renderSystem) {
		case RENDER_SYSTEM_OPENGL:
			renderSystem_ = new RenderSystemOpenGL(window.GetHandle(),
												   window.GetWidth(),
												   window.GetHeight(),
												   window.IsWindowed());
			break;

		case RENDER_SYSTEM_DIRECT3D9:
			Logger::GetInstance()->Error("Direct3D9 still not supported");
			break;

		default:
			Logger::GetInstance()->Error("Unknown render system");
			break;
	}

	return renderSystem_->initialize();
}

void Renderer::setClearColor(float red, float green, float blue) {
}

bool Renderer::beginRender() {
	return renderSystem_->beginRender();
}

void Renderer::endRender() {
	renderSystem_->endRender();
}

void Renderer::render() {
	renderSystem_->render();
}

}