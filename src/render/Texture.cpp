#include "render/Texture.h"

namespace Sketch3D {

Texture::Texture() : width_(0), height_(0), filterMode_(FILTER_MODE_POINT),
					 wrapMode_(WRAP_MODE_CLAMP)
{
}

Texture::Texture(unsigned int width, unsigned int height,
				 FilterMode_t filterMode, WrapMode_t wrapMode) : width_(width),
																 height_(height),
																 filterMode_(filterMode),
																 wrapMode_(wrapMode)
{
}

Texture::~Texture() {
}

void Texture::SetWidth(unsigned int width) {
	width_ = width;
}

void Texture::SetHeight(unsigned int height) {
	height_ = height;
}

void Texture::SetFilterMode(FilterMode_t mode) {
	filterMode_ = mode;
}

void Texture::SetWrapMode(WrapMode_t mode) {
	wrapMode_ = mode;
}

unsigned int Texture::GetWidth() const {
	return width_;
}

unsigned int Texture::GetHeight() const {
	return height_;
}

FilterMode_t Texture::GetFilterMode() const {
	return filterMode_;
}

WrapMode_t Texture::GetWrapMode() const {
	return wrapMode_;
}

}