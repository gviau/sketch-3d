#include "render/Renderer.h"

#include "render/RenderSystemOpenGL.h"
#include "system/Logger.h"

namespace Sketch3D {

Renderer Renderer::instance_;

Renderer::Renderer() : renderSystem_(NULL) {
}

Renderer::~Renderer() {
}

Renderer* Renderer::getInstance() {
	return &instance_;
}

bool Renderer::initialize(RenderSystem_t renderSystem, WindowHandle handle,
						  unsigned int width, unsigned int height, bool windowed)
{
	switch (renderSystem) {
		case RENDER_SYSTEM_OPENGL:
			renderSystem_ = new RenderSystemOpenGL(handle, width, height, windowed);
			break;

		case RENDER_SYSTEM_DIRECT3D9:
			Logger::getInstance()->error("Direct3D9 still not supported");
			break;

		default:
			Logger::getInstance()->error("Unknown render system");
			break;
	}

	return true;
}

void Renderer::setClearColor(float red, float green, float blue) {
}

bool Renderer::beginRender() {
	return true;
}

void Renderer::endRender() {
}

void Renderer::render() {
}

}