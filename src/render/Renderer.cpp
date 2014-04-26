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

Renderer* Renderer::GetInstance() {
	return &instance_;
}

bool Renderer::Initialize(RenderSystem_t renderSystem,
						  const Window& window)
{
	switch (renderSystem) {
		case RENDER_SYSTEM_OPENGL:
			renderSystem_ = new RenderSystemOpenGL(window);
			break;

		case RENDER_SYSTEM_DIRECT3D9:
			Logger::GetInstance()->Error("Direct3D9 still not supported");
			break;

		default:
			Logger::GetInstance()->Error("Unknown render system");
			break;
	}

	return renderSystem_->Initialize();
}

void Renderer::SetClearColor(float red, float green, float blue, float alpha) {
	renderSystem_->SetClearColor(red, green, blue, alpha);
}

bool Renderer::BeginRender() {
	return renderSystem_->BeginRender();
}

void Renderer::EndRender() {
	renderSystem_->EndRender();
}

void Renderer::Render() {
	renderSystem_->Render();
}

}
