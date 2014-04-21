#include "Renderer.h"

namespace Sketch3D {

Renderer Renderer::instance_;

Renderer::Renderer() {
}

Renderer::~Renderer() {
}

Renderer* Renderer::getInstance() {
	return &instance_;
}

bool Renderer::initialize(const string& name, unsigned int width,
						  unsigned int height, bool windowed)
{
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